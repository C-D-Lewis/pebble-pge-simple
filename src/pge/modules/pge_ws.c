#include "pge_ws.h"

static bool s_connected;
static PGEWSConnectedHandler *s_connection_handler;

static void in_recv_handler(DictionaryIterator *iter, void *context) {
  Tuple *t = dict_read_first(iter);
  while(t) {
    int key = (int)t->key;
    if(PGE_WS_LOGS) APP_LOG(APP_LOG_LEVEL_DEBUG, "PGE_WS: Got key %d", key);

    switch(key) {
      case PGE_WS_URL:
        s_connected = (t->value->int32 == 1);
        if(PGE_WS_LOGS) APP_LOG(APP_LOG_LEVEL_DEBUG, "PGE_WS: Connection result: %s", s_connected ? "OK" : "FAILED");
        s_connection_handler(s_connected);
        break;

      default:
        if(PGE_WS_LOGS) APP_LOG(APP_LOG_LEVEL_DEBUG, "PGE_WS: Unknown key: %d", key);
        break;
    }

    t = dict_read_next(iter);
  }
}

void pge_ws_connect(char *url, PGEWSConnectedHandler *handler) {
  // Set up callbacks
  s_connection_handler = handler;
  app_message_register_inbox_received(in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  if(PGE_WS_LOGS) APP_LOG(APP_LOG_LEVEL_DEBUG, "PGE_WS: AppMessage opened");

  // Send URL to JS
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_cstring(dict, PGE_WS_URL, url);
  app_message_outbox_send();
  if(PGE_WS_LOGS) APP_LOG(APP_LOG_LEVEL_DEBUG, "PGE_WS: URL sent");
}

void pge_ws_disconnect() {

}

bool pge_ws_is_connected() {
  return s_connected;
}

int pge_ws_get_client_id() {

}