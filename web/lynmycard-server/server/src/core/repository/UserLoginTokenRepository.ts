import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface UserLoginTokenSchema extends RowDataPacket {
  id: number;
  userId: number;
  deviceId: string;
  authToken: string;
  lastRegDate: Date;
}

export class UserLoginTokenRepository extends MySqlDao<UserLoginTokenSchema> {
  constructor(mysql: MySqlConn, data: UserLoginTokenSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["userId", "deviceId"];
  }

  getTableName() {
    return "user_login_token";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new UserLoginTokenRepository(this.getMysql(), data);
  }

  async insertOnDuplicateUpdate(
    userId: number,
    deviceId: string,
    authToken: string,
    lastRegDate: string,
  ) {
    const mysql = this.getMysql();
    let sql =
      "insert into user_login_token(userId, deviceId, authToken, lastRegDate)";
    sql += ` values(${userId}, '${deviceId}', '${authToken}', '${lastRegDate}')`;
    sql += " on duplicate key update";
    sql += ` authToken = '${authToken}', lastRegDate = '${lastRegDate}'`;
    await mysql.anyQuery(sql);

    return true;
  }
}
