import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface SupportQnaSchema extends RowDataPacket {
  id: number;
  userId: number;
  regDate: Date;
  state: number;
  qnaType: number;
  title: string;
  questionText: string;
  file1: string;
  file2: string;
  file3: string;
  answerDate: Date | null;
  answerText: string | null;
}

export class SupportQnaRepository extends MySqlDao<SupportQnaSchema> {
  constructor(mysql: MySqlConn, data: SupportQnaSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["id"];
  }

  getTableName() {
    return "qna_board";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new SupportQnaRepository(this.getMysql(), data);
  }
}
