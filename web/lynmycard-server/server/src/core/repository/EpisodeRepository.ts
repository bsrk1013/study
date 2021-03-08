import { RowDataPacket } from "mysql2/promise";
import { MySqlDao } from "../db/MySqlDao";
import { MySqlConn } from "../db/MySqlDB";

export interface EpisodeSchema extends RowDataPacket {
  id: number;
  hash: string;
  bookId: number;
  title: string;
  episodeNo: number;
  description: string;
  isHide: boolean;
  isFree: boolean;
  availableDate: Date;
  regDate: Date;
  thumbnail_s: string;
  thumbnail_m: string;
  kakaoShareImg: string;
}

export class EpisodeRepository extends MySqlDao<EpisodeSchema> {
  constructor(mysql: MySqlConn, data: EpisodeSchema | null = null) {
    super(mysql, data);
  }

  getPrimaryKey() {
    return ["id"];
  }

  getTableName() {
    return "episode";
  }

  getAutoIncrementId() {
    return "id";
  }

  getAllocator() {
    return (data) => new EpisodeRepository(this.getMysql(), data);
  }

  async findShowingEpisodes() {
    const mysql = this.getMysql();
    const rows = await mysql.query<EpisodeSchema>(
      `select * from ${this.getTableName()} where isHide = ${0}`,
    );

    return rows[0].map((x) => new EpisodeRepository(mysql, x));
  }

  async findShowingEpisodesByBookId(bookId: number) {
    const mysql = this.getMysql();
    const sql = `select * from ${this.getTableName()} where isHide = ${0} and bookId = ${bookId}`;
    const rows = await mysql.query<EpisodeSchema>(sql);

    return rows[0].map((x) => new EpisodeRepository(mysql, x));
  }

  async findLastEpisode(bookId: number) {
    const mysql = this.getMysql();
    const sql = `select * from ${this.getTableName()} where bookId = ${bookId} order by episodeNo desc limit 0, 1`;
    const rows = await mysql.query<EpisodeSchema>(sql);
    return rows[0].map((x) => new EpisodeRepository(mysql, x));
  }
}
