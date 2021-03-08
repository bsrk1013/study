import CacheRepository from "./CacheRepository";
import { BookDto } from "../store/BookStore";
import { RankingType } from "../CoreConst";

class BookRankingCacheRepository extends CacheRepository {
  constructor() {
    super("RankingDb", "Ranking:Book");
  }

  async getRanking(type: RankingType) {
    return await this.hgetAsync<BookDto[]>(type.toString());
  }

  async setRankingData(type: RankingType, datas: BookDto[]) {
    await this.hsetAsync(type.toString(), JSON.stringify(datas));
  }
}

export default new BookRankingCacheRepository();
