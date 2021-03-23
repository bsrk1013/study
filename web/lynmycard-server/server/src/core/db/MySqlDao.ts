import { RowDataPacket, OkPacket } from "mysql2/promise";
import Time from "../util/Time";
import ErrorCode from "../ErrorCode";
import Log from "../util/Log";
import { MySqlConn } from "./MySqlDB";

export interface SelectOption {
  where?: { [key: string]: any };
  offset?: number;
  limit?: number;
  orderBy?: {
    sort: "desc" | "asc";
    column: string;
  };
}

export interface DeleteOption {
  where: { [key: string]: any };
}

export abstract class MySqlDao<T extends RowDataPacket> {
  private data: T;
  private mysql: MySqlConn;

  constructor(mysql: MySqlConn, data: T | null = null) {
    this.mysql = mysql;
    if (data) {
      this.data = data;
    }
  }

  getPrimaryKey() {
    return ["id"];
  }

  getAutoIncrementId(): string | null {
    return null;
  }

  getTableName() {
    throw ErrorCode.NOT_IMPLEMENTED;
  }

  getAllocator(): (data: T) => MySqlDao<T> {
    throw ErrorCode.NOT_IMPLEMENTED;
  }

  getData() {
    return this.data;
  }

  getMysql() {
    return this.mysql;
  }

  getConnection() {
    return this.mysql.getConn();
  }

  async findAll(option: SelectOption | null = null) {
    let sql = `select * from ${this.getTableName()}`;
    if (option !== null) {
      const { where, limit, offset, orderBy } = option;
      if (where) {
        const keys = Object.keys(where);
        sql += ` where ${keys
          .map((key) => {
            const value = where[key];
            if (value instanceof Date) {
              return `${key}= "${Time.convertJsToMySqlFormat(value)}"`;
            } else if (typeof value === "boolean") {
              return `${key}= ${value === true ? 1 : 0}`;
            } else if (value instanceof Array && value.length > 0) {
              return `${key} in (${value.map((c) => c).join(",")})`;
            } else {
              return `${key}= "${value}"`;
            }
          })
          .join(" and ")}`;
      }

      if (orderBy) {
        sql += ` order by ${orderBy.column} ${orderBy.sort}`;
      }

      if (limit) {
        sql += ` limit ${limit}`;
      }

      if (offset) {
        sql += ` offset ${offset}`;
      }
    }

    const rows = await this.mysql.query<T>(sql);

    const allocator = this.getAllocator();

    let array: MySqlDao<T>[] = [];
    for (let i = 0; i < rows[0].length; i++) {
      const data = rows[0][i];
      array.push(allocator(data));
    }

    return array;
  }

  async count(option: SelectOption | null) {
    let sql = `select count(*) as count from ${this.getTableName()}`;
    if (option !== null) {
      const { where, limit, offset, orderBy } = option;
      if (where) {
        const keys = Object.keys(where);
        sql += ` where ${keys
          .map((key) => {
            const value = where[key];
            if (value instanceof Date) {
              return `${key}= "${Time.convertJsToMySqlFormat(value)}"`;
            } else if (typeof value === "boolean") {
              return `${key}= ${value === true ? 1 : 0}`;
            } else {
              return `${key}= "${value}"`;
            }
          })
          .join(" and ")}`;
      }

      if (orderBy) {
        sql += ` order by ${orderBy.column} ${orderBy.sort}`;
      }

      if (limit) {
        sql += ` limit ${limit}`;
      }

      if (offset) {
        sql += ` offset ${offset}`;
      }
    }

    const rows = await this.mysql.query<T>(sql);

    return rows[0][0]["count"] as number;
  }

  async exists(where: { [key: string]: any }) {
    this.throwIfNotValidFindOneParam(where);

    const keys = Object.keys(where);
    let innerSql = `select * from ${this.getTableName()} where ${keys
      .map((key) => {
        const value = where[key];
        if (value instanceof Date) {
          return `${key}= "${Time.convertJsToMySqlFormat(value)}"`;
        } else if (typeof value === "boolean") {
          return `${key}= ${value === true ? 1 : 0}`;
        } else if (value) {
          return `${key}= "${value}"`;
        }
      })
      .join(" and ")}`;

    const sql = `select exists(${innerSql}) as 'exists'`;
    const rows = await this.mysql.query<T>(sql);

    return rows[0][0].exists === 0 ? false : true;
  }

  async findOne(data: { [key: string]: any }) {
    this.throwIfNotValidFindOneParam(data);

    const pk = this.getPrimaryKey();
    let where = pk.map((key) => {
      const value = data[key];
      if (value instanceof Date) {
        return `${key}= "${Time.convertJsToMySqlFormat(value)}"`;
      } else if (typeof value === "boolean") {
        return `${key}= ${value === true ? 1 : 0}`;
      } else if (value) {
        return `${key}= "${value}"`;
      }
    });
    where = where.filter((c) => c !== undefined);

    const sql = `select * from ${this.getTableName()} where ${where.join(
      " and ",
    )}`;
    const rows = await this.mysql.query<T>(sql);

    if (rows[0].length === 0) {
      Log.error(sql);
      throw ErrorCode.DAO_FAILED_FIND_ONE_UNIQUE_ID;
    }

    this.data = rows[0][0];

    return this;
  }

  async findOneAllowNull(data: { [key: string]: any }) {
    this.throwIfNotValidFindOneParam(data);

    const pk = this.getPrimaryKey();
    let where = pk.map((key) => {
      const value = data[key];
      if (value instanceof Date) {
        return `${key}= "${Time.convertJsToMySqlFormat(value)}"`;
      } else if (typeof value === "boolean") {
        return `${key}= ${value === true ? 1 : 0}`;
      } else if (value) {
        return `${key}= "${value}"`;
      }
    });
    where = where.filter((c) => c !== undefined);

    const sql = `select * from ${this.getTableName()} where ${where.join(
      " and ",
    )}`;
    const rows = await this.mysql.query<T>(sql);

    if (rows[0].length === 0) {
      return null;
    } else {
      return rows[0][0];
    }
  }

  async findById(id: number) {
    const autoIncrementId = this.getAutoIncrementId();
    const sql = `select * from ${this.getTableName()} where ${autoIncrementId}= ${id}`;
    const rows = await this.mysql.query<T>(sql);

    if (rows[0].length === 0) {
      return false;
    }

    this.data = rows[0][0];

    return true;
  }

  async findAllById(ids: number[]) {
    const autoIncrementId = this.getAutoIncrementId();
    const sql = `select * from ${this.getTableName()} where ${autoIncrementId} in (${ids.join(
      ",",
    )})`;
    const rows = await this.mysql.query<T>(sql);
    const allocator = this.getAllocator();
    return rows[0].map((x) => allocator(x));
  }

  async insert() {
    if (!this.data) {
      throw ErrorCode.DAO_FAILED_INSERT_DATA_IS_NULL;
    }

    const data = this.data;
    const autoIncrementId = this.getAutoIncrementId();
    const columns = Object.keys(this.data).filter(
      (x) => !(autoIncrementId === x),
    );

    let sql = "insert into";
    sql += ` ${this.getTableName()}(${columns.join(",")})`;
    sql += ` values(${columns.map((x) => {
      if (data[x] instanceof Date) {
        return `"${Time.convertJsToMySqlFormat(data[x])}"`;
      } else if (typeof data[x] === "boolean") {
        return data[x] === true ? 1 : 0;
      } else {
        return `"${data[x]}"`;
      }
    })})`;

    const result = (await this.mysql.anyQuery(sql))[0] as OkPacket;
    if (result && autoIncrementId) {
      (this.data as any)[autoIncrementId] = result.insertId;
    }

    return this;
  }

  async insertOnDuplicate(updateData: { [key: string]: any }) {
    if (!this.data) {
      throw ErrorCode.DAO_FAILED_INSERT_DATA_IS_NULL;
    }

    const data = this.data;
    const autoIncrementId = this.getAutoIncrementId();
    const columns = Object.keys(this.data).filter(
      (x) => !(autoIncrementId === x),
    );

    let sql = "insert into";
    sql += ` ${this.getTableName()}(${columns.join(",")})`;
    sql += ` values(${columns.map((x) => {
      if (data[x] instanceof Date) {
        return `"${Time.convertJsToMySqlFormat(data[x])}"`;
      } else if (typeof data[x] === "boolean") {
        return data[x] === true ? 1 : 0;
      } else {
        return `"${data[x]}"`;
      }
    })}) on duplicate key update`;

    const updateColumns = Object.keys(updateData);
    sql += ` ${updateColumns
      .map((x) => {
        if (updateData[x] instanceof Date) {
          return `${x}="${Time.convertJsToMySqlFormat(updateData[x])}"`;
        } else if (typeof updateData[x] === "boolean") {
          return `${x}=${updateData[x] === true ? 1 : 0}`;
        } else {
          return `${x}="${updateData[x]}"`;
        }
      })
      .join(",")}`;

    const result = (await this.mysql.anyQuery(sql))[0] as OkPacket;
    if (result && autoIncrementId) {
      (this.data as any)[autoIncrementId] = result.insertId;
    }

    return this;
  }

  async update() {
    if (!this.data) {
      throw ErrorCode.DAO_FAILED_UPDATE_DATA_IS_NULL;
    }

    const data = this.data;
    const keys = Object.keys(this.data);

    let sql = `update ${this.getTableName()}`;
    sql += ` set ${keys
      .map((key) => {
        const value = data[key];
        if (value instanceof Date) {
          return `${key}= "${Time.convertJsToMySqlFormat(value)}"`;
        } else if (typeof value === "boolean") {
          return `${key}= ${value === true ? 1 : 0}`;
        } else {
          return `${key}= "${value}"`;
        }
      })
      .join(",")}`;

    const pk = this.getPrimaryKey();
    sql += ` where ${pk
      .map((key) => {
        const value = data[key];
        if (value instanceof Date) {
          return `${key}= "${Time.convertJsToMySqlFormat(value)}"`;
        } else if (typeof value === "boolean") {
          return `${key}= ${value === true ? 1 : 0}`;
        } else {
          return `${key}= "${value}"`;
        }
      })
      .join(" and ")}`;

    await this.mysql.anyQuery(sql);

    return this;
  }

  async delete() {
    if (!this.data) {
      throw ErrorCode.DAO_FAILED_DELETE_DATA_IS_NULL;
    }

    const data = this.data;
    const pk = this.getPrimaryKey();
    let sql = `delete from ${this.getTableName()}`;
    sql += ` where ${pk
      .map((key) => {
        const value = data[key];
        if (value instanceof Date) {
          return `${key}= "${Time.convertJsToMySqlFormat(value)}"`;
        } else if (typeof value === "boolean") {
          return `${key}= ${value === true ? 1 : 0}`;
        } else {
          return `${key}= "${value}"`;
        }
      })
      .join(" and ")}`;

    await this.mysql.anyQuery(sql);

    return this;
  }

  async deleteAll(option: DeleteOption) {
    const { where } = option;
    if (where) {
      const keys = Object.keys(where);
      let sql = `delete from ${this.getTableName()}`;
      sql += ` where ${keys
        .map((key) => {
          const value = where[key];
          if (value instanceof Date) {
            return `${key}= "${Time.convertJsToMySqlFormat(value)}"`;
          } else if (typeof value === "boolean") {
            return `${key}= ${value === true ? 1 : 0}`;
          } else {
            return `${key}= "${value}"`;
          }
        })
        .join(" and ")}`;

      await this.mysql.anyQuery(sql);
    }
  }

  private throwIfNotValidFindOneParam(data?: { [key: string]: string }) {
    if (data && process.env.NODE_EMV === "DEBUG") {
      const keys = Object.keys(data);
      const pk = this.getPrimaryKey();
      if (pk.some((x) => !keys.some((y) => x === y))) {
        Log.error(`A parameter have not contained primary keys`);
        throw ErrorCode.INVALID_PARAMETER;
      }
    }
  }
}
