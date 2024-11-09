#include "client.hpp"

#include <fmt/printf.h>

#include <algorithm>
#include <boost/asio/ssl/context.hpp>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

#include "constants.hpp"

namespace asio = boost::asio;

static inline void strip_quotes(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '\"'), str.end());
}

static inline void strip_escapes(std::string &str) {
    str.erase(std::remove(str.begin(), str.end(), '\\'), str.end());
}

connection_metadata::connection_metadata(int id,
                                         websocketpp::connection_hdl hdl,
                                         std::string uri)
    : m_id(id),
      m_hdl(hdl),
      m_status("Connecting"),
      m_uri(uri),
      m_server("N/A") {}

void connection_metadata::on_open(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Open";

    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    fmt::print("Connection established.\n");
}

void connection_metadata::on_fail(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Failed";

    client::connection_ptr con = c->get_con_from_hdl(hdl);
    m_server = con->get_response_header("Server");
    m_error_reason = con->get_ec().message();
}

void connection_metadata::on_close(client *c, websocketpp::connection_hdl hdl) {
    m_status = "Closed";
    client::connection_ptr con = c->get_con_from_hdl(hdl);
    std::stringstream s;
    s << "close code: " << con->get_remote_close_code() << " ("
      << websocketpp::close::status::get_string(con->get_remote_close_code())
      << "), close reason: " << con->get_remote_close_reason();
    m_error_reason = s.str();
    fmt::print("Connection closed.\n");
}

void connection_metadata::on_message(websocketpp::connection_hdl,
                                     client::message_ptr msg) {
    if (msg->get_opcode() == websocketpp::frame::opcode::text) {
        std::string s_payload = msg->get_payload();
        m_messages.push(s_payload);
    }
}

websocketpp::connection_hdl connection_metadata::get_hdl() const {
    return m_hdl;
}

int connection_metadata::get_id() const { return m_id; }

std::string connection_metadata::get_status() const { return m_status; }

void connection_metadata::record_sent_message(std::string message) {
    m_messages.push(">> " + message);
}

std::string connection_metadata::pop() {
    std::string msg = m_messages.front();
    m_messages.pop();
    return msg;
}

bool connection_metadata::empty() { return m_messages.empty(); }

context_ptr on_tls_init(const char *hostname, websocketpp::connection_hdl) {
    context_ptr ctx = websocketpp::lib::make_shared<asio::ssl::context>(
        asio::ssl::context::tlsv12);
    try {
        ctx->set_options(asio::ssl::context::default_workarounds |
                         asio::ssl::context::no_sslv2 |
                         asio::ssl::context::no_sslv3 |
                         asio::ssl::context::single_dh_use);

        ctx->set_verify_mode(asio::ssl::verify_none);
    } catch (std::exception &e) {
        fmt::print("Exception:\n{}\n", e.what());
    }
    return ctx;
}

std::string gen_nonce(int len) {
    const static std::string possible =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string nonce;

    srand(time(NULL));
    for (int i = 0; i < len; ++i) {
        int index = rand() % possible.length();
        nonce += possible[index];
    }

    return nonce;
}

websocket_endpoint::websocket_endpoint() : m_next_id(0) {
    m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
    m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

    m_endpoint.init_asio();
    m_endpoint.start_perpetual();

    m_endpoint.set_tls_init_handler(
        bind(&on_tls_init, WEBSOCKET_URL.c_str(), ::_1));

    m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(
        &client::run, &m_endpoint);
}

websocket_endpoint::~websocket_endpoint() {
    m_endpoint.stop_perpetual();

    for (con_list::const_iterator it = m_connection_list.begin();
         it != m_connection_list.end(); ++it) {
        if (it->second->get_status() != "Open") {
            // Only close open connections
            continue;
        }

        fmt::print("> Closing connection {}\n", it->second->get_id());

        websocketpp::lib::error_code ec;
        m_endpoint.close(it->second->get_hdl(),
                         websocketpp::close::status::going_away, "", ec);
        if (ec) {
            fmt::print("> Error closing connection {}: {}\n",
                       it->second->get_id(), ec.message());
        }
    }

    m_thread->join();
}

int websocket_endpoint::connect(std::string const &uri) {
    websocketpp::lib::error_code ec;

    client::connection_ptr con = m_endpoint.get_connection(uri, ec);

    if (ec) {
        fmt::print("> Connection initialization error: {}\n", ec.message());
        return -1;
    }

    int new_id = m_next_id++;
    connection_metadata::ptr metadata_ptr =
        websocketpp::lib::make_shared<connection_metadata>(
            new_id, con->get_handle(), uri);
    m_connection_list[new_id] = metadata_ptr;

    con->set_open_handler(websocketpp::lib::bind(
        &connection_metadata::on_open, metadata_ptr, &m_endpoint,
        websocketpp::lib::placeholders::_1));
    con->set_fail_handler(websocketpp::lib::bind(
        &connection_metadata::on_fail, metadata_ptr, &m_endpoint,
        websocketpp::lib::placeholders::_1));
    con->set_close_handler(websocketpp::lib::bind(
        &connection_metadata::on_close, metadata_ptr, &m_endpoint,
        websocketpp::lib::placeholders::_1));
    con->set_message_handler(
        websocketpp::lib::bind(&connection_metadata::on_message, metadata_ptr,
                               websocketpp::lib::placeholders::_1,
                               websocketpp::lib::placeholders::_2));

    m_endpoint.connect(con);

    return new_id;
}

void websocket_endpoint::close(int id, websocketpp::close::status::value code,
                               std::string reason) {
    websocketpp::lib::error_code ec;

    con_list::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        fmt::print("> No connection found with id {}\n", id);
        return;
    }

    m_endpoint.close(metadata_it->second->get_hdl(), code, reason, ec);
    if (ec) {
        fmt::print("> Error initiating close: {}\n", ec.message());
    }
}

void websocket_endpoint::send(int id, std::string message) {
    websocketpp::lib::error_code ec;

    con_list::iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        fmt::print("> No connection found with id {}\n", id);
        return;
    }

    m_endpoint.send(metadata_it->second->get_hdl(), message,
                    websocketpp::frame::opcode::text, ec);
    if (ec) {
        fmt::print("> Error sending message: {}\n", ec.message());
        return;
    }
}

void websocket_endpoint::heartbeat(int id) {
    for (;;) {
        send(id, PING);
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void websocket_endpoint::add_scopes(int id, const std::string &channel_id,
                                    const std::string &token) {
    std::string oauth_string("PASS oauth:");
    oauth_string += OAUTH;
    send(id, oauth_string);
    send(id, "NICK PaydayChaosBot");
    send(id, "JOIN #rockymoto377");
}

connection_metadata::ptr websocket_endpoint::get_metadata(int id) const {
    con_list::const_iterator metadata_it = m_connection_list.find(id);
    if (metadata_it == m_connection_list.end()) {
        return connection_metadata::ptr();
    } else {
        return metadata_it->second;
    }
}

bool websocket_endpoint::queue_empty(int id) {
    return get_metadata(id)->empty();
}
