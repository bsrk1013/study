import Express from "express";
import { ResponseScope } from "../Response";
import BookStore from "../core/store/BookStore";
import { BookSchema } from "../core/repository";
import Log from "../core/util/Log";
import { BookAuthorStore } from "../core/store";
import { DefaultSearchCount } from "../core/CoreConst";
import { BookSortMethod } from "../AppConst";
import { using } from "../core/util/Using";
import ErrorCode from "../core/ErrorCode";

const router = Express.Router();

router.get("/list", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    let book: any;
    const day = req.query.day ? Number(req.query.day) : undefined;
    const sortMethod = req.query.sort as BookSortMethod;
    if (sortMethod === "latest_episode") {
      book = await BookStore.findAllOrderByLatestEpisode(day);
    } else {
      if (day) {
        book = await BookStore.findByDay(day);
      } else {
        book = await BookStore.getShowingBooks();
      }
    }

    if (!book) {
      Log.error(
        `${scope.getName()}, book data is null, day:${day}, sort:${sortMethod}`,
      );
      scope.setResultCode(ErrorCode.NO_DATA);
      return;
    }

    scope.setData(book);
    scope.setResult(true);
  });
});

router.get("/list/main", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const books = await BookStore.getListMain();

    if (!books) {
      Log.error(`${scope.getName()}, book data is null`);
      scope.setResultCode(ErrorCode.NO_DATA);
      return;
    }

    scope.setData(books);
    scope.setResult(true);
  });
});

router.get("/list/search", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const keyword: string = req.query.keyword as string;
    const page: number = Number.parseInt(req.query.page as string);

    const bookList = await BookStore.findAllByKeyword(
      keyword,
      page * DefaultSearchCount,
      DefaultSearchCount,
    );

    const maxCount = await BookStore.countByKeyword(keyword);

    scope.setData({ bookList, maxCount });
    scope.setResult(true);
  });
});

router.get("/list/id", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const bookIdList = req.query.bookIdList as string[];
    const bookList = await BookStore.findById(
      bookIdList.map((x) => parseInt(x)),
    );

    scope.setData(bookList);
    scope.setResult(true);
  });
});

router.post("/:bookId/author", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const authorId = req.body.authorId as number;
    const bookId = parseInt(req.params.bookId);

    await BookAuthorStore.insert(bookId, authorId);

    scope.setResult(true);
  });
});

router.get("/:id", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const bookId = parseInt(req.params.id);
    const book = await BookStore.findOneAndAllForeignData(bookId);

    scope.setData(book);
    scope.setResult(true);
  });
});

router.post("/", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    await BookStore.insert(
      <BookSchema>{
        title: req.body.title,
        description: req.body.description,
        genres: req.body.genres,
        day: req.body.day,
        isHide: req.body.isHide,
      },
      req.body.authorIds,
    );

    scope.setResult(true);
  });
});

export default router;
