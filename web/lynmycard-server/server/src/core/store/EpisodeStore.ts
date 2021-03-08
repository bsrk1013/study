import {
  EpisodeRepository,
  EpisodeSchema,
  UserPageViewLogSchema,
  UserPageViewLogRepository,
} from "../repository";
import crypto from "crypto";
import Time from "../util/Time";
import { UserStore } from ".";
import { IRankable } from "./RankingStore";
import BookStore from "./BookStore";
import { BookDto } from "./BookStore";
import {
  EpisodeContentsRepository,
  EpisodeContentsSchema,
} from "../repository";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import ErrorCode from "../ErrorCode";
import {
  ViewDetailLogSchema,
  ViewDetailLogRepository,
  ClientViewDetailLogSchema,
} from "../repository/ViewDetailLogRepository";
import { DefaultSearchCount } from "../CoreConst";
import { patch } from "../util/Patch";
import { BookRepository, BookSchema } from "../repository/BookRepository";

export class EpisodeDto {
  id: number;
  hash: string;
  bookId: number;
  title: string;
  episodeNo: number;
  description: string;
  isFree: boolean;
  regDate: Date;
  availableDate: Date;
  thumbnail_s: string;
  thumbnail_m: string;
  kakaoShareImg: string;

  constructor(entity: EpisodeSchema) {
    this.id = entity.id;
    this.hash = entity.hash;
    this.bookId = entity.bookId;
    this.title = entity.title;
    this.episodeNo = entity.episodeNo;
    this.description = entity.description;
    this.isFree = entity.isFree;
    this.regDate = entity.regDate;
    this.availableDate = entity.availableDate;
    this.thumbnail_s = entity.thumbnail_s;
    this.thumbnail_m = entity.thumbnail_m;
    this.kakaoShareImg = entity.kakaoShareImg;
  }
}

export class UserPageViewLogDto {
  id: number;
  bookId: number;
  episodeId: number;
  userId: number;
  regDate: Date;

  constructor(entity: UserPageViewLogSchema) {
    this.id = entity.id;
    this.bookId = entity.bookId;
    this.episodeId = entity.episodeId;
    this.userId = entity.userId;
    this.regDate = entity.regDate;
  }
}

export class ViewDetailLogDto {
  id: number;
  regDate: Date;
  userId: number;
  ip: string;
  bookId: number;
  episodeId: number;
  subscriptionPid: number;
  subscriptionName: string;

  constructor(entity: ViewDetailLogSchema) {
    this.id = entity.id;
    this.regDate = entity.regDate;
    this.userId = entity.userId;
    this.ip = entity.ip;
    this.bookId = entity.bookId;
    this.episodeId = entity.episodeId;
    this.subscriptionPid = entity.subscriptionPid;
    this.subscriptionName = entity.subscriptionName;
  }
}

export class ClientViewDetailLogDto {
  id: number;
  regDate: Date;
  userId: number;
  ip: string;
  // 웹툰 정보
  bookId: number;
  bookTitle: string;
  bookHash: string;
  bookThumbnailM: string;

  // 에피소드 정보
  episodeId: number;
  episodeTitle: string;
  episodeNo: number;

  // 구독권 정보
  subscriptionPid: number;
  subscriptionName: string;

  constructor(entity: ClientViewDetailLogSchema) {
    this.id = entity.id;
    this.regDate = entity.regDate;
    this.userId = entity.userId;
    this.ip = entity.ip;

    this.bookId = entity.bookId;
    this.bookTitle = entity.bookTitle;
    this.bookHash = entity.bookHash;
    this.bookThumbnailM = entity.bookThumbnailM;

    this.episodeId = entity.episodeId;
    this.episodeTitle = entity.episodeTitle;
    this.episodeNo = entity.episodeNo;

    this.subscriptionPid = entity.subscriptionPid;
    this.subscriptionName = entity.subscriptionName;
  }
}

export class EpisodeBookDto implements IRankable {
  id: number;
  hash: string;
  book: BookDto;
  title: string;
  episodeNo: number;
  description: string;
  isFree: boolean;
  regDate: Date;

  constructor(entity: EpisodeDto, book: BookDto) {
    this.id = entity.id;
    this.hash = entity.hash;
    this.book = book;
    this.title = entity.title;
    this.episodeNo = entity.episodeNo;
    this.description = entity.description;
    this.isFree = entity.isFree;
    this.regDate = entity.regDate;
  }
}

class EpisodeFilesDto {
  episodeId: number;
  files: string;
  no: number;
  updateDate: Date;
  constructor(entity: EpisodeContentsSchema) {
    this.episodeId = entity.episodeId;
    this.files = entity.files;
    this.no = entity.no;
    this.updateDate = entity.updateDate;
  }
}

class EpisodeDetailDto
  implements Omit<EpisodeDto & { book: BookDto }, "bookId"> {
  id: number;
  hash: string;
  title: string;
  episodeNo: number;
  description: string;
  isFree: boolean;
  regDate: Date;
  availableDate: Date;
  thumbnail_s: string;
  thumbnail_m: string;
  kakaoShareImg: string;
  book: BookDto;

  constructor(episodeEntity: EpisodeSchema, bookEntity: BookSchema) {
    this.id = episodeEntity.id;
    this.hash = episodeEntity.hash;
    this.title = episodeEntity.title;
    this.episodeNo = episodeEntity.episodeNo;
    this.description = episodeEntity.description;
    this.isFree = episodeEntity.isFree;
    this.regDate = episodeEntity.regDate;
    this.availableDate = episodeEntity.availableDate;
    this.thumbnail_s = episodeEntity.thumbnail_s;
    this.thumbnail_m = episodeEntity.thumbnail_m;
    this.kakaoShareImg = episodeEntity.kakaoShareImg;
    this.book = new BookDto(bookEntity);
  }
}

namespace EpisodeStore {
  export async function getShowingEpisodes() {
    return await using(
      new MySqlConn(getShowingEpisodes.name),
      async (mysql) => {
        return await getShowingEpisodesNoneUsing(mysql);
      },
    );
  }

  export async function getShowingEpisodesNoneUsing(mysql: MySqlConn) {
    const entities = await new EpisodeRepository(mysql).findShowingEpisodes();

    if (!entities) {
      throw ErrorCode.EPISODE_NOT_FOUND;
    }

    return entities.map((x) => new EpisodeDto(x.getData()));
  }

  export async function getShowingEpisodeBooks() {
    return await using(
      new MySqlConn(getShowingEpisodeBooks.name),
      async (mysql) => {
        return await getShowingEpisodeBooksNoneUsing(mysql);
      },
    );
  }

  export async function getShowingEpisodeBooksNoneUsing(mysql: MySqlConn) {
    const episodes = await getShowingEpisodesNoneUsing(mysql);
    const bookTable = await BookStore.getShowingBookTableNoneUsing(mysql);

    return episodes
      .filter((x) => bookTable.hasOwnProperty(x.bookId))
      .map((x) => new EpisodeBookDto(x, bookTable[x.bookId]));
  }

  export async function getShowingEpisodesByBook(bookId: number) {
    return await using(
      new MySqlConn(getShowingEpisodesByBook.name),
      async (mysql) => {
        return await getShowingEpisodesByBookNoneUsing(mysql, bookId);
      },
    );
  }

  export async function getShowingEpisodesByBookNoneUsing(
    mysql: MySqlConn,
    bookId: number,
  ) {
    const bookEntity = await BookStore.findOneNoneUsing(mysql, bookId);
    if (bookEntity.isHide) {
      throw "This book cannot be used.";
    }

    const entities = await new EpisodeRepository(
      mysql,
    ).findShowingEpisodesByBookId(bookId);

    return entities.map((x) => new EpisodeDto(x.getData()));
  }

  export async function getEpisodesByBooks(bookIds: number[]) {
    return await using(
      new MySqlConn(getEpisodesByBooks.name),
      async (mysql) => {
        const entities = await new EpisodeRepository(mysql).findAll({
          where: {
            bookId: bookIds,
          },
        });

        return entities.map((entity) => new EpisodeDto(entity.getData()));
      },
    );
  }

  export async function getLastEpisode(bookId: number) {
    return await using(new MySqlConn(getLastEpisode.name), async (mysql) => {
      const bookEntity = await BookStore.findOneNoneUsing(mysql, bookId);
      if (bookEntity.isHide) {
        throw "This book cannot be used.";
      }

      const entities = await new EpisodeRepository(mysql).findLastEpisode(
        bookId,
      );

      return entities.map((x) => new EpisodeDto(x.getData()));
    });
  }

  export async function findOne(id: number) {
    return await using(new MySqlConn(findOne.name), async (mysql) => {
      const entity = await new EpisodeRepository(mysql).findOne({ id });
      return new EpisodeDto(entity.getData());
    });
  }

  export async function insert(data: EpisodeSchema) {
    await using(new MySqlConn(insert.name), async (mysql) => {
      const regDate = Time.getNow();
      const entity = new EpisodeRepository(mysql, {
        ...data,
        hash: crypto
          .createHash("sha256")
          .update(data.title + regDate)
          .digest("hex")
          .substr(0, 10),
        regDate,
      });
      await entity.insert();
    });
  }

  export async function emitPageViewLog(
    bookId: number,
    episodeId: number,
    userId: number,
  ) {
    return await using(new MySqlConn(emitPageViewLog.name), async (mysql) => {
      const newEntity = await new UserPageViewLogRepository(mysql, <
        UserPageViewLogSchema
      >{
        bookId,
        episodeId,
        userId,
        regDate: new Date(),
      });

      await newEntity.insert();

      return true;
    });
  }

  export async function deleteViewDetailLog(userId: number) {
    return await using(
      new MySqlConn(deleteViewDetailLog.name),
      async (mysql) => {
        let now = new Date();
        const expired = now.setDate(now.getDate() - 7);
        await mysql.query(
          `delete from view_detail_log where userId = ${userId} and regDate <= '${Time.convertJsToMySqlFormat(
            Time.convertToDateFromMsTs(expired),
          )}'`,
        );
      },
    );
  }

  export async function emitViewDetailLog(
    userId: number,
    ip: string,
    bookId: number,
    episodeId: number,
    subscriptionPid: number,
    subscriptionName: string,
  ) {
    return await using(new MySqlConn(emitViewDetailLog.name), async (mysql) => {
      const newEntity = await new ViewDetailLogRepository(mysql, <
        ViewDetailLogSchema
      >{
        regDate: new Date(),
        userId,
        ip,
        bookId,
        episodeId,
        subscriptionPid,
        subscriptionName,
      });

      await newEntity.insert();

      return true;
    });
  }

  export async function findAllViewDetailLog(userId: number, page: number) {
    return await using(
      new MySqlConn(findAllViewDetailLog.name),
      async (mysql) => {
        const rows = await mysql.query<ClientViewDetailLogSchema>(
          `
      SELECT a.id, a.regDate, a.userId, a.ip, 
      a.bookId, b.title AS 'bookTitle', b.hash AS 'bookHash',
      b.thumbnail_m as 'bookThumbnailM',
      a.episodeId, c.title AS 'episodeTitle', c.episodeNo,
      a.subscriptionPid, a.subscriptionName
      FROM view_detail_log a
      LEFT JOIN book b ON a.bookId = b.id
      LEFT JOIN episode c ON a.episodeId = c.id
      WHERE a.userId = ${userId}
      ORDER BY regDate DESC
      LIMIT ${page * DefaultSearchCount}, ${DefaultSearchCount}
      `,
        );

        const list = rows[0].map(
          (entity) => new ClientViewDetailLogDto(entity),
        );

        var maxCount = await new ViewDetailLogRepository(mysql).count({
          where: { userId },
        });

        return { list, maxCount };
      },
    );
  }

  export async function existsPageViewLog(
    bookId: number,
    episodeId: number,
    userId: number,
  ) {
    return await using(new MySqlConn(existsPageViewLog.name), async (mysql) => {
      const exists = await new UserPageViewLogRepository(mysql).exists({
        bookId: bookId,
        episodeId: episodeId,
        userId: userId,
      });

      return exists;
    });
  }

  export async function findOnePageViewLog(
    bookId: number,
    episodeId: number,
    userId: number,
  ) {
    return await using(
      new MySqlConn(findOnePageViewLog.name),
      async (mysql) => {
        const entity = await new UserPageViewLogRepository(mysql).findOne({
          bookId: bookId,
          episodeId: episodeId,
          userId: userId,
        });

        if (!entity.getData()) {
          return null;
        }

        return new UserPageViewLogDto(entity.getData());
      },
    );
  }

  export async function findAllViewLog(userId: number) {
    return await using(new MySqlConn(findAllViewLog.name), async (mysql) => {
      const entities = await new UserPageViewLogRepository(mysql).findAll({
        where: {
          userId: userId,
        },
      });

      return entities.map((entity) => new UserPageViewLogDto(entity.getData()));
    });
  }

  export async function findAllViewLogByBookIds(
    userId: number,
    bookIds: number[],
  ) {
    return await using(
      new MySqlConn(findAllViewLogByBookIds.name),
      async (mysql) => {
        const entities = await new UserPageViewLogRepository(mysql).findAll({
          where: {
            userId: userId,
            bookId: bookIds,
          },
        });

        return entities.map(
          (entity) => new UserPageViewLogDto(entity.getData()),
        );
      },
    );
  }

  function VerifyUserId(userId: string) {
    if (!userId) {
      return false;
    }

    if (!UserStore.exists(userId)) {
      return false;
    }

    return true;
  }

  export async function getContents(episodeId: number) {
    return await using(new MySqlConn(getContents.name), async (mysql) => {
      const entities = await new EpisodeContentsRepository(mysql).findAll({
        where: {
          episodeId: episodeId,
        },
      });

      if (entities.length === 0) {
        throw ErrorCode.EPISODE_NOT_FOUND;
      }

      return entities.map((x) => new EpisodeFilesDto(x.getData()));
    });
  }

  export async function getEpisodeDetail(id: number) {
    return await using(new MySqlConn(getEpisodeDetail.name), async (mysql) => {
      const episodeEntity = new EpisodeRepository(mysql);
      if (!(await episodeEntity.findById(id))) {
        throw ErrorCode.EPISODE_NOT_FOUND;
      }

      const bookEntity = new BookRepository(mysql);
      if (!(await bookEntity.findById(episodeEntity.getData().bookId))) {
        throw ErrorCode.BOOK_NOT_FOUND;
      }

      return new EpisodeDetailDto(
        episodeEntity.getData(),
        bookEntity.getData(),
      );
    });
  }
}

patch({ EpisodeStore });

export default EpisodeStore;
