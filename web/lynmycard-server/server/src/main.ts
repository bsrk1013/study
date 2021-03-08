import express from "express";
import cors, { CorsOptions } from "cors";
import bodyParser from "body-parser";
import * as routes from "./routes";
import cookieParser from "cookie-parser";
import { CookieJsonParser } from "./middleware/CookieJsonParser";
import { ServerConfig, Configs } from "./Config";
import MySqlDB, { MySqlConn } from "./core/db/MySqlDB";
import Log from "./core/util/Log";
import TimeScheduler from "./core/util/TimeSchedule/TimeScheduler";
import SessionManager from "./core/util/TimeUpdater/SessionManager";
import AuthManager, { AuthInfo } from "./core/util/TimeUpdater/AuthManager";
import AutopayManager from "./core/util/TimeUpdater/AutopayManager";
import { AuthorStore } from "./core/store";
import DormancyManager from "./core/util/TimeUpdater/DormancyManager";

const app = express();

const corsOptions = <CorsOptions>{
  origin: function (origin, callback) {
    if (
      !origin ||
      ServerConfig.Origins[0] === "*" ||
      ServerConfig.Origins.indexOf(origin) !== -1
    ) {
      callback(null, true);
    } else {
      callback(new Error("Not allowed by CORS"));
    }
  },
  credentials: true,
};

const allowCorsOptions = <CorsOptions>{
  origin: "*",
  credentials: true,
};

app.set("views", __dirname + "/views");
app.set("view engine", "ejs");
app.engine("html", require("ejs").renderFile);

app.use(
  cors((req, callback) => {
    Log.debug(req.host);
    if (ServerConfig.AllowUrl.indexOf(req.url) !== -1) {
      callback(null, allowCorsOptions);
    } else {
      callback(null, corsOptions);
    }
  }),
);
app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json({ type: "application/json" }));

// MiddleWare
app.use(express.static("public"));
app.use(cookieParser());
// 쿠키에서 필요한 데이터들은 아래의 미들웨어에서 세팅해주자
app.use(CookieJsonParser);
// app.use("/api/book/", DeviceAuthMW);

// Routing
app.use("/api/episode", routes.Episode);
app.use("/api/book", routes.Book);
app.use("/api/author", routes.Author);
app.use("/api/user", routes.User);
app.use("/api/favorite", routes.Favorite);
app.use("/api/notification", routes.Notification);
app.use("/api/auth", routes.Auth);
app.use("/api/device", routes.Device);
app.use("/api/popular", routes.Popular);
app.use("/api/banner", routes.Banner);
app.use("/api/product", routes.Product);
app.use("/api/ranking", routes.Ranking);
app.use("/api/verifier", routes.Verifier);
app.use("/api/support", routes.SupportQna);
app.use("/mycard", routes.MyCard);

// MongoDB.Instance();
MySqlDB.Instance();
//TimeScheduler.Instance();
//SessionManager.Instance();
AuthManager.Instance();
AutopayManager.Instance();
DormancyManager.Instance();

console.log(`build target:${Configs.env}`);

process.on("uncaughtException", (error) => {
  // Log.error(`uncaughtException, error:${error}`);
});

process.on("unhandledRejection", (error) => {
  // Log.error(`unhandledRejection, error:${error}`);
});

app.listen(ServerConfig.Port, () => {
  console.log(`[express] hosted at ${ServerConfig.Host}:${ServerConfig.Port}`);
});
