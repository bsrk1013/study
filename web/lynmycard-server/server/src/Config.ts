import yaml from "js-yaml";
import fs from "fs";
import path from "path";

interface Configs {
  env: string;
  aws: AwsConfig;
  mysql: MySqlConfig;
  redis: RedisConfig;
  server: ServerConfig;
  telegrambot: TelegramBotConfig;
  payletter: PayLetterConfig;
}

interface RedisConfig {
  Host: string;
  Port: number;
  Password: string;
  LogDb: number;
  EventDb: number;
}

interface ServerConfig {
  Port: number;
  Host: string;
  ClientOrigin: string;
  Origins: string[];
  AllowUrl: string[];
}

interface MySqlConfig {
  host: string;
  user: string;
  password: string;
  port: number;
  database: string;
  connectionLimit: number;
  stream: any;
}

interface AwsConfig {
  accessKeyId: string;
  secretAccessKey: string;
  region: string;
  sesSourceMail: string;
}

interface TelegramBotConfig {
  token: string;
  chatid: string;
}

interface PayLetterConfig {
  BaseUrl: string;
  ApiUrl: string;
  ApiBillingKey: string;
  ApiReadKey: string;
  ClientId: string;
  KeyId: string;
  KeyVersion: string;
  KeyValue: string;
}

class Config {
  static Load<T>(confgFile: string) {
    const fullpath = path.resolve(__dirname, confgFile);
    return yaml.safeLoad(fs.readFileSync(fullpath, "utf8")) as T;
  }
}

export const Configs = Config.Load<Configs>("../config.yml");
export const RedisConfig = Configs.redis;
export const ServerConfig = Configs.server;
export const MySqlConfig = Configs.mysql;
export const AwsConfig = Configs.aws;
export const PayLetterConfig = Configs.payletter;
