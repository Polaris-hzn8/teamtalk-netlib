/**
 * @author: luochenhao
 * @email: lch2022fox@163.com
 * @time: Sun 03 May 2026 23:16:50 CST
 * @brief: HTTP解析器包装类实现
 */

#include <string>
#include <cstring>
#include <cstdlib>
#include <teamtalk/imcore/string/string.h>
#include <teamtalk/imcore/http_client/http_parser.h>
#include <teamtalk/imcore/http_client/http_parser_wrapper.h>

namespace teamtalk::imcore::http_client {

#define MAX_REFERER_LEN 32

CHttpParserWrapper::CHttpParserWrapper()
  : m_http_parser(std::make_unique<http_parser>()), m_settings(std::make_unique<http_parser_settings>()) {}

CHttpParserWrapper::~CHttpParserWrapper() = default;

void CHttpParserWrapper::ParseHttpContent(const char* buf, uint32_t len) {
  http_parser_init(m_http_parser.get(), HTTP_REQUEST);
  memset(m_settings.get(), 0, sizeof(http_parser_settings));

  m_settings->on_url = OnUrl;
  m_settings->on_header_field = OnHeaderField;
  m_settings->on_header_value = OnHeaderValue;
  m_settings->on_headers_complete = OnHeadersComplete;
  m_settings->on_body = OnBody;
  m_settings->on_message_complete = OnMessageComplete;

  m_settings->object = this;
  m_http_parser->data = this;

  m_read_all = false;
  m_read_referer = false;
  m_read_forward_ip = false;
  m_read_user_agent = false;
  m_read_content_type = false;
  m_read_content_len = false;
  m_read_host = false;
  m_total_length = 0;
  m_content_len = 0;

  m_url.clear();
  m_body_content.clear();
  m_referer.clear();
  m_forward_ip.clear();
  m_user_agent.clear();
  m_content_type.clear();
  m_host.clear();

  http_parser_execute(m_http_parser.get(), m_settings.get(), buf, len);
}

char CHttpParserWrapper::GetMethod() {
  return static_cast<char>(m_http_parser->method);
}

int CHttpParserWrapper::OnUrl(http_parser* parser, const char* at, size_t length, void* obj) {
  (void)parser;
  static_cast<CHttpParserWrapper*>(obj)->SetUrl(at, length);
  return 0;
}

int CHttpParserWrapper::OnHeaderField(http_parser* parser, const char* at, size_t length, void* obj) {
  (void)parser;
  auto* w = static_cast<CHttpParserWrapper*>(obj);
  const std::string field(at, length);

  if (!w->HasReadReferer()) {
    if (teamtalk::imcore::string::str_iequals(field, "Referer")) {
      w->SetReadReferer(true);
    }
  }

  if (!w->HasReadForwardIP()) {
    if (teamtalk::imcore::string::str_iequals(field, "X-Forwarded-For")) {
      w->SetReadForwardIP(true);
    }
  }

  if (!w->HasReadUserAgent()) {
    if (teamtalk::imcore::string::str_iequals(field, "User-Agent")) {
      w->SetReadUserAgent(true);
    }
  }

  if (!w->HasReadContentType()) {
    if (teamtalk::imcore::string::str_iequals(field, "Content-Type")) {
      w->SetReadContentType(true);
    }
  }

  if (!w->HasReadContentLen()) {
    if (teamtalk::imcore::string::str_iequals(field, "Content-Length")) {
      w->SetReadContentLen(true);
    }
  }

  if (!w->HasReadHost()) {
    if (teamtalk::imcore::string::str_iequals(field, "Host")) {
      w->SetReadHost(true);
    }
  }
  return 0;
}

int CHttpParserWrapper::OnHeaderValue(http_parser* parser, const char* at, size_t length, void* obj) {
  (void)parser;
  auto* w = static_cast<CHttpParserWrapper*>(obj);

  if (w->IsReadReferer()) {
    size_t referer_len = (length > MAX_REFERER_LEN) ? MAX_REFERER_LEN : length;
    w->SetReferer(at, referer_len);
    w->SetReadReferer(false);
  }

  if (w->IsReadForwardIP()) {
    w->SetForwardIP(at, length);
    w->SetReadForwardIP(false);
  }

  if (w->IsReadUserAgent()) {
    w->SetUserAgent(at, length);
    w->SetReadUserAgent(false);
  }

  if (w->IsReadContentType()) {
    w->SetContentType(at, length);
    w->SetReadContentType(false);
  }

  if (w->IsReadContentLen()) {
    std::string strContentLen(at, length);
    w->SetContentLen(static_cast<uint32_t>(atoi(strContentLen.c_str())));
    w->SetReadContentLen(false);
  }

  if (w->IsReadHost()) {
    w->SetHost(at, length);
    w->SetReadHost(false);
  }
  return 0;
}

int CHttpParserWrapper::OnHeadersComplete(http_parser* parser, void* obj) {
  auto* w = static_cast<CHttpParserWrapper*>(obj);
  w->SetTotalLength(parser->nread + static_cast<uint32_t>(parser->content_length));
  return 0;
}

int CHttpParserWrapper::OnBody(http_parser* parser, const char* at, size_t length, void* obj) {
  (void)parser;
  static_cast<CHttpParserWrapper*>(obj)->SetBodyContent(at, length);
  return 0;
}

int CHttpParserWrapper::OnMessageComplete(http_parser* parser, void* obj) {
  (void)parser;
  static_cast<CHttpParserWrapper*>(obj)->SetReadAll();
  return 0;
}

}  // namespace teamtalk::imcore::http_client
