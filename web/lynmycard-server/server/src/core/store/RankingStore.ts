import { RankingType } from "../CoreConst";
import {
  RankingRoundSchema,
  RankingRoundRepository,
  EpisodeRankingRepository,
  EpisodeRankingSchema,
  UserPageViewLogSchema,
  BookRankingSchema,
  BookRankingRepository,
  EpisodeRankingCacheRepository,
  BookRankingCacheRepository,
} from "../repository";
import { EpisodeStore, UserPageViewLogStore } from ".";
import { EpisodeBookDto } from "./EpisodeStore";
import { using } from "../util/Using";
import Log from "../util/Log";
import BookStore, { BookDto } from "./BookStore";
import { MySqlConn } from "../db/MySqlDB";
import { BookRepository } from "../repository/BookRepository";
import { patch } from "../util/Patch";

export interface BookListDto {
  isEnd: boolean;
  books: BookDto[];
}

export interface IRankable {
  id: number;
}

interface EntryInfo {
  obj: IRankable;
  viewCount: number;
}

namespace RankingStore {
  export async function getAll() {
    return {
      webToonEpisodeHot: await EpisodeRankingCacheRepository.getRanking(
        RankingType.Hourly,
      ),
      webToonEpisodeDaily: await EpisodeRankingCacheRepository.getRanking(
        RankingType.Daily,
      ),
      webToonEpisodeWeekly: await EpisodeRankingCacheRepository.getRanking(
        RankingType.Weekly,
      ),
      webToonHot: await BookRankingCacheRepository.getRanking(
        RankingType.Hourly,
      ),
      webToonDaily: await BookRankingCacheRepository.getRanking(
        RankingType.Daily,
      ),
      webToonWeekly: await BookRankingCacheRepository.getRanking(
        RankingType.Weekly,
      ),
    };
  }

  export async function getBookListAll(type: number) {
    return await using(new MySqlConn(getBookListAll.name), async (mysql) => {
      const list = await new BookRepository(mysql).findAll({
        orderBy: {
          sort: "asc",
          column: "id",
        },
      });
      return <BookListDto>{
        isEnd: true,
        books: list.map((x) => new BookDto(x.getData())),
      };
    });
  }

  export async function getBookList(type: number, page: number) {
    const count = 2;
    return await using(new MySqlConn(getBookList.name), async (mysql) => {
      const list = await new BookRepository(mysql).findAll({
        orderBy: {
          sort: "asc",
          column: "id",
        },
      });
      return <BookListDto>{
        isEnd: page * count + count >= list.length,
        books: list
          .slice(page * count, page * count + count)
          .map((x) => new BookDto(x.getData())),
      };
    });
  }

  export async function executeEpisodeRanking(
    type: RankingType,
    startDate: Date,
    endDate: Date,
  ) {
    Log.debug("executeEpisodeRanking");
    Log.debug(`type: ${type.toString()}`);
    return await using(
      new MySqlConn(executeEpisodeRanking.name),
      async (mysql) => {
        const newRound = await new RankingRoundRepository(mysql, <
          RankingRoundSchema
        >{
          type,
          startDate: startDate,
          endDate: endDate,
          regDate: new Date(),
        });
        await newRound.insert();

        const items = await getEpisodeEntryItemsNoneUsing(
          mysql,
          startDate,
          endDate,
        );
        const rankingDatas = items
          .sort((x, y) => y.viewCount - x.viewCount) // 조회순으로 내림차순 정렬
          .map((x, i) => {
            return <EpisodeRankingSchema>{
              rankingRoundId: newRound.getData().id,
              rankingNo: i,
              episodeId: items[i].obj.id,
              score: items[i].viewCount,
              type,
            };
          });

        await Promise.all(
          rankingDatas.map((x) =>
            new EpisodeRankingRepository(mysql, x).insert(),
          ),
        );

        const episodeEntities = await EpisodeStore.getShowingEpisodeBooksNoneUsing(
          mysql,
        );
        const episodeEntitiesOrderById = episodeEntities.sort(
          (x, y) => x.id - y.id,
        );
        const sortedEntities: EpisodeBookDto[] = Array<EpisodeBookDto>(
          rankingDatas.length,
        );
        rankingDatas
          .sort((x, y) => x.episodeId - y.episodeId)
          .map(
            (x, i) =>
              (sortedEntities[x.rankingNo] = episodeEntitiesOrderById[i]),
          );

        await EpisodeRankingCacheRepository.setRankingData(
          type,
          sortedEntities,
        );
      },
    );
  }

  export async function executeBookRanking(
    type: RankingType,
    startDate: Date,
    endDate: Date,
  ) {
    Log.debug("executeBookRanking");
    Log.debug(`type: ${type.toString()}`);
    return await using(
      new MySqlConn(executeBookRanking.name),
      async (mysql) => {
        const newRound = new RankingRoundRepository(mysql, <RankingRoundSchema>{
          type,
          startDate: startDate,
          endDate: endDate,
          regDate: new Date(),
        });
        await newRound.insert();

        const items = await getBookEntryItemsNoneUsing(
          mysql,
          startDate,
          endDate,
        );
        const rankingDatas = items
          .sort((x, y) => y.viewCount - x.viewCount) // 조회순으로 내림차순 정렬
          .map((x, i) => {
            return <BookRankingSchema>{
              rankingRoundId: newRound.getData().id,
              rankingNo: i,
              bookId: items[i].obj.id,
              score: items[i].viewCount,
              type,
            };
          });

        await Promise.all(
          rankingDatas.map((x) => new BookRankingRepository(mysql, x).insert()),
        );

        const bookEntities = await BookStore.getShowingBooksNoneUsing(mysql);
        const bookEntitiesOrderById = bookEntities.sort((x, y) => x.id - y.id);
        const sortedEntities: BookDto[] = Array<BookDto>(rankingDatas.length);
        rankingDatas
          .sort((x, y) => x.bookId - y.bookId)
          .map(
            (x, i) => (sortedEntities[x.rankingNo] = bookEntitiesOrderById[i]),
          );

        await BookRankingCacheRepository.setRankingData(type, sortedEntities);
      },
    );
  }
}

async function getEpisodeEntryItems(startDate: Date, endDate: Date) {
  Log.debug(`Start date: ${startDate}, End date: ${endDate}`);
  return await using(
    new MySqlConn(getEpisodeEntryItems.name),
    async (mysql) => {
      return await getEpisodeEntryItemsNoneUsing(mysql, startDate, endDate);
    },
  );
}

async function getEpisodeEntryItemsNoneUsing(
  mysql: MySqlConn,
  startDate: Date,
  endDate: Date,
) {
  Log.debug(`Start date: ${startDate}, End date: ${endDate}`);
  return calculateEpisodeCount(
    await EpisodeStore.getShowingEpisodeBooksNoneUsing(mysql),
    await UserPageViewLogStore.getLogsNoneUsing(mysql, startDate, endDate),
  );
}

async function getBookEntryItems(startDate: Date, endDate: Date) {
  Log.debug(`Start date: ${startDate}, End date: ${endDate}`);
  return await using(new MySqlConn(getBookEntryItems.name), async (mysql) => {
    return await getBookEntryItemsNoneUsing(mysql, startDate, endDate);
  });
}

async function getBookEntryItemsNoneUsing(
  mysql: MySqlConn,
  startDate: Date,
  endDate: Date,
) {
  Log.debug(`Start date: ${startDate}, End date: ${endDate}`);
  return calculateBookCount(
    await BookStore.getShowingBooksNoneUsing(mysql),
    await UserPageViewLogStore.getLogsNoneUsing(mysql, startDate, endDate),
  );
}

function calculateEpisodeCount(
  source: EpisodeBookDto[],
  viewLogs: UserPageViewLogSchema[],
) {
  return calculateCount<EpisodeBookDto>((x) => x.episodeId, source, viewLogs);
}

function calculateBookCount(
  source: BookDto[],
  viewLogs: UserPageViewLogSchema[],
) {
  return calculateCount<BookDto>((x) => x.bookId, source, viewLogs);
}

function calculateCount<T extends IRankable>(
  countingTargetSelector: (item: UserPageViewLogSchema) => number,
  source: T[],
  viewLogs: UserPageViewLogSchema[],
) {
  Log.debug(`Source count: ${source.length}, Log count: ${viewLogs.length}`);
  const blackboard: { [key: string]: number } = {};

  source.forEach((x) => (blackboard[x.id] = 0));

  viewLogs.forEach((x) => {
    const id = countingTargetSelector(x);
    const count = blackboard[id];
    blackboard[id] = count + 1;
  });

  return source.map(
    (x) =>
      <EntryInfo>{
        obj: x,
        viewCount: blackboard[x.id],
      },
  );
}

patch({ RankingStore });

export default RankingStore;
