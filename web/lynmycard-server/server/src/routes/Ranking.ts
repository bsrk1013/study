import Express from "express";
import { RankingStore } from "../core/store";
import { using } from "../core/util/Using";
import { ResponseScope } from "../Response";
import { BookListDto } from "../core/store/RankingStore";

const router = Express.Router();

router.get("/all", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const result = await RankingStore.getAll();

    scope.setData(result);
    scope.setResult(true);
  });
});

router.get("/episode/hourly", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    scope.setResult(true);
  });
});

router.get("/episode/daily", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    scope.setResult(true);
  });
});

router.get("/episode/weekly", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    scope.setResult(true);
  });
});

router.get("/book/hourly", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    scope.setResult(true);
  });
});

router.get("/book/daily", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    scope.setResult(true);
  });
});

router.get("/book/weekly", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    scope.setResult(true);
  });
});

router.get("/book/list", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const type: number = Number.parseInt(req.query.type as string);

    let bookListDto: BookListDto;
    if (req.query.page) {
      const page: number = Number.parseInt(req.query.page as string);

      bookListDto = await RankingStore.getBookList(type, page);
    } else {
      bookListDto = await RankingStore.getBookListAll(type);
    }

    scope.setData(bookListDto);
    scope.setResult(true);
  });
});

export default router;
