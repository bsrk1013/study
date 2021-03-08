import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface EpisodeContentsSchema extends RowDataPacket {
  id: number;
  episodeId: number;
  files: string;
  no: number;
  updateDate: Date;
}

export class EpisodeContentsRepository extends MySqlDao<EpisodeContentsSchema> {
  constructor(mysql: MySqlConn, data: EpisodeContentsSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["episodeId"];
  }

  getTableName() {
    return "episode_contents";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new EpisodeContentsRepository(this.getMysql(), data);
  }
}
