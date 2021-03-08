import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface SupportQnaTypeSchema extends RowDataPacket {
  id: number;
  qnaType: number;
  typeName: string;
}

export class SupportQnaTypeRepository extends MySqlDao<SupportQnaTypeSchema> {
  constructor(mysql: MySqlConn, data: SupportQnaTypeSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["id"];
  }

  getTableName() {
    return "qna_type";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new SupportQnaTypeRepository(this.getMysql(), data);
  }
}
