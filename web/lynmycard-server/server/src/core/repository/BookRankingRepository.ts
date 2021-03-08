import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface BookRankingSchema extends RowDataPacket {
  id: number;
  rankingRoundId: number;
  rankingNo: number;
  bookId: number;
  score: number;
  type: number; // See AppConst.RankingType
}

export class BookRankingRepository extends MySqlDao<BookRankingSchema> {
  constructor(mysql: MySqlConn, data: BookRankingSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["bookId"];
  }

  getTableName() {
    return "book_ranking";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new BookRankingRepository(this.getMysql(), data);
  }
}
