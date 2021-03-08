import * as redis from "redis";

declare module "redis" {
  export interface RedisClient extends NodeJS.EventEmitter {
    // String
    getAsync(key: string): Promise<string>;
    getbitAsync(key: string, offset: number): Promise<number>;
    getrangeAsync(key: string, start: number, end: number): Promise<string>;
    getsetAsync(key: string, value: string): Promise<string>;

    // Hashes
    hdelAsync(
      key: string,
      ...args: Array<string> | Array<string>[]
    ): Promise<number>;
    hexistsAsync(key: string, field: string): Promise<number>;
    hgetAsync(key: string, field: string): Promise<string>;
    hgetallAsync(key: string): Promise<{ [key: string]: string }>;
    hsetAsync(key: string, field: string, value: string): Promise<number>;
    hmsetAsync(
      key: string,
      ...args: Array<string> | Array<string>[]
    ): Promise<number>;
    hsetnxAsync(key: string, field: string, value: string): Promise<number>;
    hlenAsync(key: string): Promise<number>;
    hincrbyAsync(key:string, field:string, increment:number):Promise<number>;

    // Increase key value
    incrAsync(key: string): Promise<number>;
    incrbyAsync(key: string, increment: number): Promise<number>;
    incrbyfloatAsync(key: string, increment: number): Promise<number>;

    // List
    lindexAsync(key: string, index: number): Promise<string>;
    linsertAsync(
      key: string,
      dir: "BEFORE" | "AFTER",
      pivot: string,
      value: string,
    ): Promise<string>;
    llenAsync(key: string): Promise<number>;
    lpopAsync(key: string): Promise<number>;
    lpushAsync(
      key: string,
      ...args: Array<string | number> | Array<string | number>[]
    ): Promise<number>;
    lpushxAsync(key: string, value: string): Promise<number>;
    lrangeAsync(key: string, start: number, stop: number): Promise<string>;
    lremAsync(key: string, count: number, value: string): Promise<number>;
    lsetAsync(key: string, index: number, value: string): Promise<"OK">;
    ltrimAsync(key: string, start: number, stop: number): Promise<"OK">;
    rpopAsync(key: string): Promise<number>;
    rpushAsync(
      key: string,
      ...args: Array<string | number> | Array<string | number>[]
    ): Promise<number>;

    // Expire(TTL)
    persistAsync(key: string): Promise<number>;
    pexpireAsync(key: string, milliseconds: number): Promise<number>;
    pexpireatAsync(key: string, millisecondsTimestamp: number): Promise<number>;
    psetexAsync(
      key: string,
      milliseconds: number,
      value: string,
    ): Promise<"OK">;
    pttlAsync(key: string): Promise<number>;
    ttlAsync(key: string): Promise<number>;
    expireAsync(key: string, seconds: number): Promise<number>;
    expiratAsync(key: string, timestamp: number): Promise<number>;

    // Sets
    saddAsync(
      key: string,
      ...args: Array<string> | Array<string>[]
    ): Promise<number>;
    scardAsync(key: string): Promise<number>;
    smembersAsync(key: string): Promise<string[]>;
    sismemberAsync(key: string): Promise<number>;
    sremAsync(key: string, value: string | string[]): Promise<number>;

    // Sorted Sets
    zaddAsync(
      key: string,
      ...args: Array<number | string> | Array<number | string>[]
    ): Promise<number>;
    zcardAsync(key: string): Promise<number>;
    zcountAsync(
      key: string,
      min: number | string,
      max: number | string,
    ): Promise<number>;
    zincrbyAsync(
      key: string,
      increment: number,
      member: string,
    ): Promise<string>;
    zrangeAsync(key: string, start: number, stop: number): Promise<string[]>;
    zrevrangeAsync(key: string, start: number, stop: number): Promise<string[]>;
    zrangebyscoreAsync(
      key: string,
      min: number | string,
      max: number | string,
    ): Promise<string[]>;
    zrangebyscoreAsync(
      key: string,
      min: number | string,
      max: number | string,
      withscores: string,
    ): Promise<string[]>;
    zrangebyscoreAsync(
      key: string,
      min: number | string,
      max: number | string,
      limit: string,
      offset: number,
      count: number,
    ): Promise<string[]>;
    zrangebyscoreAsync(
      key: string,
      min: number | string,
      max: number | string,
      withscores: string,
      limit: string,
      offset: number,
      count: number,
    ): Promise<string[]>;
    zrankAsync(key: string, member: string): Promise<number>;
    zremAsync(
      key: string,
      ...args: Array<string> | Array<string>[]
    ): Promise<number>;
    zscoreAsync(key: string, member: string): Promise<string>;
  }
}
