#include <websocketpp/client.hpp>
#include <websocketpp/common/memory.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/config/asio_client.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context>
    context_ptr;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

class connection_metadata {
   public:
    typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;
    connection_metadata(int, websocketpp::connection_hdl, std::string);
    void on_open(client *, websocketpp::connection_hdl);
    void on_fail(client *, websocketpp::connection_hdl);
    void on_close(client *, websocketpp::connection_hdl);
    void on_message(websocketpp::connection_hdl, client::message_ptr);
    websocketpp::connection_hdl get_hdl() const;
    int get_id() const;
    std::string get_status() const;
    void record_sent_message(std::string);
    std::string pop();
    bool empty();

   private:
    int m_id;
    websocketpp::connection_hdl m_hdl;
    std::string m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;
    std::queue<std::string> m_messages;
    const char *__RTD_ID__ = "ee89352d-66a5-451c-9540-c800d7b8af6f";
};

class websocket_endpoint {
   public:
    websocket_endpoint();
    ~websocket_endpoint();
    int connect(const std::string &);
    void close(int, websocketpp::close::status::value, std::string);
    void send(int, std::string);
    void heartbeat(int);
    void add_scopes(int, const std::string &, const std::string &);
    bool queue_empty(int);
    std::string pop(int);
    connection_metadata::ptr get_metadata(int) const;

   private:
    typedef std::map<int, connection_metadata::ptr> con_list;

    client m_endpoint;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

    con_list m_connection_list;
    int m_next_id;

    const char *PING = "{\"type\":\"PING\"}";
    const char *RECONNECT = "{\"type\":\"RECONNECT\"}";
    const char *OAUTH = "cn6esspapxrgavnarxfl5j3ttgd4si";
};
