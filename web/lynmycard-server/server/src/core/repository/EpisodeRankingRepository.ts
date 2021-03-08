import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface EpisodeRankingSchema extends RowDataPacket {
  id: number;
  episodeId: number;
  rankingNo: number;
  rankingRoundId: number;
  score: number;
  type: number; // See AppConst.RankingType
}

export class EpisodeRankingRepository extends MySqlDao<EpisodeRankingSchema> {
  constructor(mysql: MySqlConn, data: EpisodeRankingSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["episodeId"];
  }

  getTableName() {
    return "episode_ranking";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new EpisodeRankingRepository(this.getMysql(), data);
  }
}
