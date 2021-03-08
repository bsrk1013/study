import { Configs } from "../../Config";
import axios from "axios";

/*
Reset = "\x1b[0m"
Bright = "\x1b[1m"
Dim = "\x1b[2m"
Underscore = "\x1b[4m"
Blink = "\x1b[5m"
Reverse = "\x1b[7m"
Hidden = "\x1b[8m"

FgBlack = "\x1b[30m"
FgRed = "\x1b[31m"
FgGreen = "\x1b[32m"
FgYellow = "\x1b[33m"
FgBlue = "\x1b[34m"
FgMagenta = "\x1b[35m"
FgCyan = "\x1b[36m"
FgWhite = "\x1b[37m"

BgBlack = "\x1b[40m"
BgRed = "\x1b[41m"
BgGreen = "\x1b[42m"
BgYellow = "\x1b[43m"
BgBlue = "\x1b[44m"
BgMagenta = "\x1b[45m"
BgCyan = "\x1b[46m"
BgWhite = "\x1b[47m"
*/

const Log = {
  debug(message: any) {
    // 디버그 메시지
    console.log("\x1b[32m%s\x1b[0m", message);
  },

  info(message: any) {
    // 인포 메시지
    console.log("\x1b[36m%s\x1b[0m", message);
  },

  warn(message: any) {
    console.warn("\x1b[33m%s\x1b[0m", message);
    var warn = new Error();
    console.warn("\x1b[33m%s\x1b[0m", warn.stack);

    Log.sendTelegramBot(message, warn.stack);
  },

  error(message: any) {
    // 애러 메시지
    console.error("\x1b[31m%s\x1b[0m", message);
    var err = new Error();
    console.error("\x1b[31m%s\x1b[0m", err.stack);

    Log.sendTelegramBot(message, err.stack);
  },

  sendTelegramBot(message: any, stack: string | undefined) {
    if (Configs.env.localeCompare("development") === 0) {
      return;
    }

    var text = `build target:${Configs.env}\n${message}\n${stack}`;
    var encoded = encodeURI(text);
    var url = `https://api.telegram.org/bot${Configs.telegrambot.token}/sendmessage?chat_id=${Configs.telegrambot.chatid}&text=${encoded}`;
    axios.get(url);
  },

  makeMessageFromReq(req: any, error: any) {
    return `url: ${req.originalUrl}\nheader: ${JSON.stringify(
      req.headers,
      null,
      2,
    )}\n${error}`;
  },
};

export default Log;
