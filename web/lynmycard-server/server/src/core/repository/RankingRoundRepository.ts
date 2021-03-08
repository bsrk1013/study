import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface RankingRoundSchema extends RowDataPacket {
  id: number;
  type: number; // See AppConst.RankingType
  startDate: Date;
  endDate: Date;
  regDate: Date;
}

export class RankingRoundRepository extends MySqlDao<RankingRoundSchema> {
  constructor(mysql: MySqlConn, data: RankingRoundSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["id"];
  }

  getTableName() {
    return "ranking_round";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new RankingRoundRepository(this.getMysql(), data);
  }
}
