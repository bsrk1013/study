import MySql, { RowDataPacket } from "mysql2";
import { Pool, PoolConnection } from "mysql2/promise";
import { MySqlConfig } from "../../Config";
import { IDisposable } from "../util/Using";
import Log from "../util/Log";

export default class MySqlDB {
  private static instance: MySqlDB = new MySqlDB();
  private pool: Pool;

  static Instance(): MySqlDB {
    return this.instance;
  }

  constructor() {
    Log.info("================== MySqlDB2 Config ==================");
    console.log(MySqlConfig);
    Log.info("====================================================");

    Log.info("MySqlDB Config Load...");

    this.init();

    Log.info("Mysql started...");
  }

  private async init() {
    this.pool = MySql.createPool(MySqlConfig).promise();
  }

  async getConn() {
    return await this.pool.getConnection();
  }
}

export class MySqlConn implements IDisposable {
  private name: string;
  private conn: PoolConnection;
  private needTransaction: boolean;
  private slowQueryTime: number;
  private startTime: number;
  private sqlList: string[];

  constructor(
    name: string,
    needTransaction: boolean = false,
    slowQueryTime: number = 1000,
  ) {
    this.name = name;
    this.slowQueryTime = slowQueryTime;
    this.needTransaction = needTransaction;
    this.sqlList = [];
  }

  getConn() {
    return this.conn;
  }

  public async query<T extends RowDataPacket>(sql: string) {
    this.sqlList.push(sql);

    return await this.conn.query<T[]>(sql).catch((error) => {
      Log.error(`name:${this.name}\n${this.getSql()}\n${error}`);
    });
  }

  public async anyQuery(sql: string) {
    this.sqlList.push(sql);

    return await this.conn.query(sql).catch((error) => {
      Log.error(`name:${this.name}\n${this.getSql()}\n${error}`);
    });
  }

  private getSql() {
    let sql = "";
    for (let i = 0; i < this.sqlList.length; i++) {
      sql += `${this.sqlList[i]}\n`;
    }
    return sql;
  }

  async init() {
    this.conn = await MySqlDB.Instance().getConn();
    this.startTime = Date.now();

    if (this.needTransaction) {
      await this.conn.beginTransaction();
    }
  }

  async dispose(valid: boolean, code: number) {
    const now = Date.now();
    const elapsed = now - this.startTime;

    if (this.slowQueryTime <= elapsed) {
      Log.warn(
        `slow query... elapsed:${elapsed}\nname:${this.name}\n${this.getSql()}`,
      );
    }

    try {
      if (valid) {
        if (this.needTransaction) {
          await this.conn.commit();
        }
      } else {
        if (this.needTransaction) {
          await this.conn.rollback();
        }
      }
    } catch (error) {
      Log.error(`name:${this.name}\n${this.getSql()}\n${error}`);
    } finally {
      this.conn.release();
    }
  }
}
