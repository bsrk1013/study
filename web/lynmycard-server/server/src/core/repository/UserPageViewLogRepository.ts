import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import Time from "../util/Time";
import { MySqlConn } from "../db/MySqlDB";

export interface UserPageViewLogSchema extends RowDataPacket {
  id: number;
  bookId: number;
  episodeId: number;
  userId: number;
  regDate: Date;
}

export class UserPageViewLogRepository extends MySqlDao<UserPageViewLogSchema> {
  constructor(mysql: MySqlConn, data: UserPageViewLogSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["userId", "episodeId", "bookId"];
  }

  getTableName() {
    return "user_page_view_log";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new UserPageViewLogRepository(this.getMysql(), data);
  }

  async selectLogs(startDate: Date, endDate: Date) {
    const mysql = this.getMysql();
    const rows = await mysql.query<UserPageViewLogSchema>(
      `select * from ${this.getTableName()} where regDate between '${Time.convertJsToMySqlFormat(
        startDate,
      )}' and '${Time.convertJsToMySqlFormat(endDate)}'`,
    );

    return rows[0].map((x) => new UserPageViewLogRepository(mysql, x));
  }
}
