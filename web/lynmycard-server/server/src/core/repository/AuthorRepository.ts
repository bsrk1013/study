import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface AuthorSchema extends RowDataPacket {
  id: number;
  name: string;
  regDate: Date;
}

export class AuthorRepository extends MySqlDao<AuthorSchema> {
  constructor(mysql: MySqlConn, data: AuthorSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["id"];
  }

  getTableName() {
    return "author";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new AuthorRepository(this.getMysql(), data);
  }
}
