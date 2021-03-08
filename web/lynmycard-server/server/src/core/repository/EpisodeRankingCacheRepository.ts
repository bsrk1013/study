import CacheRepository from "./CacheRepository";
import { EpisodeBookDto } from "../store/EpisodeStore";
import { RankingType } from "../CoreConst";

class EpisodeRankingCacheRepository extends CacheRepository {
  constructor() {
    super("RankingDb", "Ranking:Episode");
  }

  async getRanking(type: RankingType) {
    return await this.hgetAsync<EpisodeBookDto[]>(type.toString());
  }

  async setRankingData(type: RankingType, datas: EpisodeBookDto[]) {
    await this.hsetAsync(type.toString(), JSON.stringify(datas));
  }
}

export default new EpisodeRankingCacheRepository();
