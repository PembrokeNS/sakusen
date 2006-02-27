#include "ui/key.h"

#include "gnu_extensions.h"
#include "stringutils.h"

using namespace sakusen;
using namespace __gnu_cxx;

namespace tedomari {
namespace ui {

hash_map<String, Key, StringHash> initializeKeyLookup()
{
  hash_map<String, Key, StringHash> l;
  for (int i=0; i<K_Unknown; ++i) {
    l[getName(static_cast<Key>(i))] = static_cast<Key>(i);
  }
  /* Also some alternative names */
  l["cr"] = K_RETURN;
  l["enter"] = K_RETURN;
  l["kpcr"] = K_KP_ENTER;
  l["kpreturn"] = K_KP_ENTER;
  return l;
}

hash_map<String, Key, StringHash> keyLookup = initializeKeyLookup();

Key getKey(const String& name)
{
  hash_map<String, Key, StringHash>::iterator key = keyLookup.find(name);
  if (key == keyLookup.end()) {
    return K_Unknown;
  } else {
    return key->second;
  }
}

String getName(Key k)
{
  switch (k) {
    case K_BACKSPACE:
      return "backspace";
    case K_TAB:
      return "tab";
    case K_CLEAR:
      return "clear";
    case K_RETURN:
      return "return";
    case K_PAUSE:
      return "pause";
    case K_ESCAPE:
      return "esc";
    case K_SPACE:
      return "space";
    case K_EXCLAIM:
      return "!";
    case K_QUOTEDBL:
      return "\"";
    case K_HASH:
      return "#";
    case K_DOLLAR:
      return "$";
    case K_AMPERSAND:
      return "&";
    case K_QUOTE:
      return "'";
    case K_LEFTPAREN:
      return "(";
    case K_RIGHTPAREN:
      return ")";
    case K_ASTERISK:
      return "*";
    case K_PLUS:
      return "+";
    case K_COMMA:
      return ",";
    case K_MINUS:
      return "-";
    case K_PERIOD:
      return ".";
    case K_SLASH:
      return "/";
    case K_0:
      return "0";
    case K_1:
      return "1";
    case K_2:
      return "2";
    case K_3:
      return "3";
    case K_4:
      return "4";
    case K_5:
      return "5";
    case K_6:
      return "6";
    case K_7:
      return "7";
    case K_8:
      return "8";
    case K_9:
      return "9";
    case K_COLON:
      return ":";
    case K_SEMICOLON:
      return ";";
    case K_LESS:
      return "<";
    case K_EQUALS:
      return "=";
    case K_GREATER:
      return ">";
    case K_QUESTION:
      return "?";
    case K_AT:
      return "@";
    case K_LEFTBRACKET:
      return "[";
    case K_BACKSLASH:
      return "\\";
    case K_RIGHTBRACKET:
      return "]";
    case K_CARET:
      return "^";
    case K_UNDERSCORE:
      return "_";
    case K_BACKQUOTE:
      return "`";
    case K_a:
      return "a";
    case K_b:
      return "b";
    case K_c:
      return "c";
    case K_d:
      return "d";
    case K_e:
      return "e";
    case K_f:
      return "f";
    case K_g:
      return "g";
    case K_h:
      return "h";
    case K_i:
      return "i";
    case K_j:
      return "j";
    case K_k:
      return "k";
    case K_l:
      return "l";
    case K_m:
      return "m";
    case K_n:
      return "n";
    case K_o:
      return "o";
    case K_p:
      return "p";
    case K_q:
      return "q";
    case K_r:
      return "r";
    case K_s:
      return "s";
    case K_t:
      return "t";
    case K_u:
      return "u";
    case K_v:
      return "v";
    case K_w:
      return "w";
    case K_x:
      return "x";
    case K_y:
      return "y";
    case K_z:
      return "z";
    case K_DELETE:
      return "delete";
    case K_KP0:
      return "kp0";
    case K_KP1:
      return "kp1";
    case K_KP2:
      return "kp2";
    case K_KP3:
      return "kp3";
    case K_KP4:
      return "kp4";
    case K_KP5:
      return "kp5";
    case K_KP6:
      return "kp6";
    case K_KP7:
      return "kp7";
    case K_KP8:
      return "kp8";
    case K_KP9:
      return "kp9";
    case K_KP_PERIOD:
      return "kp.";
    case K_KP_DIVIDE:
      return "kp/";
    case K_KP_MULTIPLY:
      return "kp*";
    case K_KP_MINUS:
      return "kp-";
    case K_KP_PLUS:
      return "kp+";
    case K_KP_ENTER:
      return "kpenter";
    case K_KP_EQUALS:
      return "kp=";
    case K_UP:
      return "up";
    case K_DOWN:
      return "down";
    case K_RIGHT:
      return "right";
    case K_LEFT:
      return "left";
    case K_INSERT:
      return "insert";
    case K_HOME:
      return "home";
    case K_END:
      return "end";
    case K_PAGEUP:
      return "pageup";
    case K_PAGEDOWN:
      return "pagedown";
    case K_F1:
      return "f1";
    case K_F2:
      return "f2";
    case K_F3:
      return "f3";
    case K_F4:
      return "f4";
    case K_F5:
      return "f5";
    case K_F6:
      return "f6";
    case K_F7:
      return "f7";
    case K_F8:
      return "f8";
    case K_F9:
      return "f9";
    case K_F10:
      return "f10";
    case K_F11:
      return "f11";
    case K_F12:
      return "f12";
    case K_F13:
      return "f13";
    case K_F14:
      return "f14";
    case K_F15:
      return "f15";
    case K_NUMLOCK:
      return "numlock";
    case K_CAPSLOCK:
      return "capslock";
    case K_SCROLLOCK:
      return "scrolllock";
    case K_RSHIFT:
      return "rshift";
    case K_LSHIFT:
      return "lshift";
    case K_RCTRL:
      return "rctrl";
    case K_LCTRL:
      return "lctrl";
    case K_RALT:
      return "ralt";
    case K_LALT:
      return "lalt";
    case K_RMETA:
      return "rmeta";
    case K_LMETA:
      return "lmeta";
    case K_LSUPER:
      return "lsuper";
    case K_RSUPER:
      return "rsuper";
    case K_MODE:
      return "mode";
    case K_HELP:
      return "help";
    case K_PRINT:
      return "print";
    case K_SYSREQ:
      return "sysreq";
    case K_BREAK:
      return "break";
    case K_MENU:
      return "menu";
    case K_POWER:
      return "power";
    case K_EURO:
      return "euro";
    case K_MOUSELEFT:
      return "mleft";
    case K_MOUSERIGHT:
      return "mright";
    case K_MOUSEMIDDLE:
      return "mmiddle";
    case K_MOUSEWHEELUP:
      return "mwheelup";
    case K_MOUSEWHEELDOWN:
      return "mwheeldown";
    default:
      return "";
  }
}

}}

