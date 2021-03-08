import {
  BookRepository,
  BookAuthorRepository,
  BookSchema,
} from "../repository";
import AuthorStore from "./AuthorStore";
import crypto from "crypto";
import BookAuthorStore from "./BookAuthorStore";
import Time from "../util/Time";
import { IRankable } from "./RankingStore";
import { using } from "../util/Using";
import { MySqlConn } from "../db/MySqlDB";
import EpisodeStore from "./EpisodeStore";
import { patch } from "../util/Patch";

export class BookDto implements IRankable {
  id: number;
  hash: string;
  title: string;
  authors: string[];
  genres: number[];
  description: string;
  day: number[];
  isHide: boolean;
  regDate: Date;
  thumbnail_s: string;
  thumbnail_m: string;
  bannerImg: string;
  bannerImg_mobile: string;
  kakaoShareImg: string;
  orderNo: number;
  isShowMain: boolean;

  constructor(entity: BookSchema) {
    this.id = entity.id;
    this.hash = entity.hash;
    this.title = entity.title;
    this.authors = entity.authors.split(",");
    this.genres = entity.genres.split(",").map((x) => parseInt(x));
    this.description = entity.description;
    this.day = entity.day.split(",").map((x) => parseInt(x));
    this.isHide = entity.isHide !== 0;
    this.regDate = entity.regDate;
    this.thumbnail_s = entity.thumbnail_s;
    this.thumbnail_m = entity.thumbnail_m;
    this.bannerImg = entity.bannerImg;
    this.bannerImg_mobile = entity.bannerImg_mobile;
    this.kakaoShareImg = entity.kakaoShareImg;
    this.orderNo = entity.orderNo;
    this.isShowMain = entity.isShowMain !== 0;
  }

  toDao(mysql: MySqlConn) {
    return new BookRepository(mysql, <BookSchema>{
      id: this.id,
      hash: this.hash,
      title: this.title,
      authors: this.authors.join(","),
      genres: this.genres.join(","),
      description: this.description,
      day: this.day.join(","),
      isHide: this.isHide ? 1 : 0,
      regDate: this.regDate,
      thumbnail_s: this.thumbnail_s,
      thumbnail_m: this.thumbnail_m,
      bannerImg: this.bannerImg,
      bannerImg_mobile: this.bannerImg_mobile,
      orderNo: this.orderNo,
      isShowMain: this.isShowMain ? 1 : 0,
    });
  }
}

namespace BookStore {
  export interface InsertParam {
    id: string;
    hash: string;
    title: string;
    description: string;
    authorIds: string[];
    genres: number[];
    day: number;
    recommendedAge: number;
    regDate: number;
  }

  export async function findOneAndAllForeignData(id: number) {
    return await using(
      new MySqlConn(findOneAndAllForeignData.name),
      async (mysql) => {
        const bookEntity = await new BookRepository(mysql).findOne({ id });

        const bookAuthorEntities = await new BookAuthorRepository(
          mysql,
        ).findAll({
          where: {
            bookId: id,
          },
        });

        const episodes = await EpisodeStore.getShowingEpisodesByBookNoneUsing(
          mysql,
          id,
        );
        const authors = await AuthorStore.findByIdNoneUsing(
          mysql,
          bookAuthorEntities.map((x) => x.getData().authorId),
        );

        return {
          ...new BookDto(bookEntity.getData()),
          episodes,
          authors: authors.map((x) => x.name),
        };
      },
    );
  }

  export async function findOne(id: number) {
    return await using(new MySqlConn(findOne.name), async (mysql) => {
      return await findOneNoneUsing(mysql, id);
    });
  }

  export async function findOneNoneUsing(mysql: MySqlConn, id: number) {
    const bookEntity = await new BookRepository(mysql).findOne({ id });

    const bookAuthorEntities = await new BookAuthorRepository(mysql).findAll({
      where: {
        bookId: id,
      },
    });

    const authors = await AuthorStore.findByIdNoneUsing(
      mysql,
      bookAuthorEntities.map((x) => x.getData().authorId),
    );

    return {
      ...new BookDto(bookEntity.getData()),
      authors,
    };
  }

  export async function getShowingBooks() {
    return await using(new MySqlConn(getShowingBooks.name), async (mysql) => {
      return await getShowingBooksNoneUsing(mysql);
    });
  }

  export async function getShowingBooksNoneUsing(mysql: MySqlConn) {
    const entities = await new BookRepository(mysql).findAll({
      where: {
        isHide: false,
      },
      orderBy: {
        sort: "desc",
        column: "orderNo",
      },
    });

    return entities.map((x) => new BookDto(x.getData()));
  }

  // key(bookId):value(BookDto)로 이루어진 object를 반환합니다
  export async function getShowingBookTable() {
    const table: { [key: string]: BookDto } = {};
    return await using(
      new MySqlConn(getShowingBookTable.name),
      async (mysql) => {
        return await getShowingBookTableNoneUsing(mysql);
      },
    );
  }

  export async function getShowingBookTableNoneUsing(mysql: MySqlConn) {
    const table: { [key: string]: BookDto } = {};
    const books = await getShowingBooksNoneUsing(mysql);
    books.forEach((x) => (table[x.id] = x));

    return table;
  }

  export async function findByDay(day: number) {
    return await using(new MySqlConn(findByDay.name), async (mysql) => {
      const list = await getShowingBooksNoneUsing(mysql);
      const listOfDay = list.filter((x) => x.day.includes(day));
      return await Promise.all(
        listOfDay.map(async (x) => {
          const authors = await BookAuthorStore.findAuthorsByBookIdNoneUsing(
            mysql,
            x.id,
          );
          return {
            ...x,
            authors,
          };
        }),
      );
    });
  }

  export async function findById(ids: number[]) {
    return await using(new MySqlConn(findById.name), async (mysql) => {
      const entities = await new BookRepository(mysql).findAllById(ids);
      return entities.map((x) => {
        return new BookDto(x.getData());
      });
    });
  }

  export async function getListMain() {
    return await using(new MySqlConn(getListMain.name), async (mysql) => {
      return (await findAllOrderByLatestEpisodeNoneUsing(mysql)).filter(
        (x) => !x.isHide && x.isShowMain,
      );
    });
  }

  export async function findAllOrderByLatestEpisode(day?: number) {
    return await using(
      new MySqlConn(findAllOrderByLatestEpisode.name),
      async (mysql) => {
        return await findAllOrderByLatestEpisodeNoneUsing(mysql, day);
      },
    );
  }

  export async function findAllOrderByLatestEpisodeNoneUsing(
    mysql: MySqlConn,
    day?: number,
  ) {
    const entities = await new BookRepository(
      mysql,
    ).findAllOrderByLatestEpisode(day);
    return entities.map((x) => new BookDto(x));
  }

  // TODO: mysql migration
  export async function findAllByKeyword(
    keyword: string,
    start: number,
    count: number,
  ) {
    return await using(new MySqlConn(findAllByKeyword.name), async (mysql) => {
      const verifyKeyword =
        keyword.length > 0 && keyword.localeCompare(" ") !== 0;

      if (!verifyKeyword) {
        throw "Invalid keyword";
      }

      const searchResult = await new BookRepository(mysql).findAllByKeyword(
        keyword,
        start,
        count,
      );

      return searchResult
        .filter((x) => x.getData().isHide === 0)
        .map((c) => new BookDto(c.getData()));
    });
  }

  // TODO: mysql migration
  export async function countByKeyword(keyword: string) {
    // const verifyKeyword =
    //   keyword.length > 0 && keyword.localeCompare(" ") !== 0;

    // if (!verifyKeyword) {
    //   throw "Invalid keyword";
    // }

    // const query = [
    //   { title: { $regex: keyword } },
    //   { authors: { $regex: keyword } },
    // ];
    // const count = await BookRepository.count({ $or: query });

    return null;
  }

  export async function insert(data: BookSchema, authorIds: number[]) {
    if (data.title.length === 0) {
      throw "title.length === 0";
    }

    return await using(new MySqlConn(insert.name), async (mysql) => {
      const regDate = Time.getNow();
      const authorDtos = await AuthorStore.findByIdNoneUsing(mysql, authorIds);
      const bookEntity = new BookRepository(mysql, {
        ...data,
        hash: crypto
          .createHash("sha256")
          .update(data.title + regDate)
          .digest("hex")
          .substr(0, 10),
        authors: authorDtos.map((x) => x.name).join(","),
        regDate,
      });

      await bookEntity.insert();

      await Promise.all(
        authorIds.map((authorId) =>
          BookAuthorStore.insertNoneUsing(
            mysql,
            bookEntity.getData().id,
            authorId,
          ),
        ),
      );
    });
  }
}

patch({ BookStore });

export default BookStore;
