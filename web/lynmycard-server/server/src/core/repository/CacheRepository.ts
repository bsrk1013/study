import * as redis from "redis";
import * as bluebird from "bluebird";
import { RedisConfig } from "../../Config";
import "redis-ts-async";
import Log from "../util/Log";

bluebird.Promise.promisifyAll((<any>redis).RedisClient.prototype);
bluebird.Promise.promisifyAll((<any>redis).Multi.prototype);

export interface ZSetValueScore {
  value: string;
  score: number;
}

class CacheRepository {
  static IncrementTable = "IncrementTable";
  private database: string;
  private key: string;
  private client: redis.RedisClient;

  constructor(database: string, key: string) {
    const db = RedisConfig[database];
    this.database = database;
    this.key = key;
    this.client = redis.createClient({
      host: RedisConfig.Host,
      port: RedisConfig.Port,
      db: db,
      password: RedisConfig.Password,
    });
  }

  getClient() {
    return this.client;
  }

  convertHash<T>(rawData: string): T {
    const data = JSON.parse(rawData);
    return data as T;
  }

  convertHashes<T>(rawDatas: { [key: string]: string }): T[] {
    let dataArray: T[] = [];

    for (const key in rawDatas) {
      const rawData = rawDatas[key];
      const data = JSON.parse(rawData) as T;
      dataArray.push(data);
    }

    return dataArray;
  }

  convertString<T>(rawData: string): T {
    const data = JSON.parse(rawData);
    return data as T;
  }

  async zaddAsyncHelper(datas: ZSetValueScore[]) {
    let dataArray: (string | number)[] = [];

    for (const index in datas) {
      const data = datas[index];
      dataArray.push(data.score);
      dataArray.push(data.value);
    }

    const result = await this.client.zaddAsync(this.key, dataArray);
    return result;
  }

  async hsetAsync(field: string, value: string, suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    return await this.client.hsetAsync(newKey, field, value);
  }

  async hdelAsync(field: string, suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    return await this.client.hdelAsync(newKey, field);
  }

  async hexistsAsync(field: string, suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    return await this.client.hexistsAsync(newKey, field);
  }

  async hgetAsync<T>(field: string, suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    const value = await this.client.hgetAsync(newKey, field);
    return this.convertHash<T>(value);
  }

  async hgetallAsync<T>(suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    const values = await this.client.hgetallAsync(newKey);
    return this.convertHashes<T>(values);
  }

  async hincrbyAsync(field: string, value: number) {
    return await this.client.hincrbyAsync(this.key, field, value);
  }

  async sismemberAsync(value: string, suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    return new Promise<number>((resolve, reject) => {
      this.client.sismember(newKey, value, (error, result) => {
        if (error) {
          reject(error);
        } else {
          resolve(result);
        }
      });
    });
  }

  async setAsync(value: string, suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    return await this.client.saddAsync(newKey, value);
  }
  async sremAsync(value: string, suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    return await this.client.sremAsync(newKey, value);
  }

  async getsetAsync<T>(value: string, suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    const rawData = await this.client.getsetAsync(newKey, value);
    return this.convertString(rawData);
  }

  async smembersAsync<T>(suffixKey = "") {
    const newKey = `${this.key}${suffixKey}`;
    const rawDatas = await this.client.smembersAsync(newKey);
    return rawDatas.map((c) => {
      return this.convertString<T>(c);
    });
  }

  async transactionIncrEventAction(action: (id: number) => Promise<boolean>) {
    try {
      let id = 0;

      const lastId = await this.getLatestEventId();
      if (lastId !== -1 && lastId !== Number.MAX_SAFE_INTEGER) {
        id = lastId + 1;
      } else {
        await this.client.hsetAsync(
          CacheRepository.IncrementTable,
          this.key,
          id.toString(),
        );
      }

      const result = await action(id);

      if (result) {
        await this.client.hincrbyAsync(
          CacheRepository.IncrementTable,
          this.key,
          1,
        );
      }
    } catch (error) {}
  }

  async getLatestEventId() {
    const value = await this.client.hgetAsync(
      CacheRepository.IncrementTable,
      this.key,
    );
    const id = Number.parseInt(value ? value : "-1");

    return id;
  }
}

export default CacheRepository;
