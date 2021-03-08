import Express from "express";
import FavoriteStore from "../core/store/FavoriteStore";
import { using } from "../core/util/Using";
import { ResponseScope } from "../Response";

const router = Express.Router();

router.put("/book", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = req.body.userId;
    const bookId = req.body.bookId;

    let remove = true;
    if (userId > 0) {
      remove = await FavoriteStore.updateBookFavorite(userId, bookId);
    }

    scope.setData({ remove });
    scope.setResult(true);
  });
});

router.get("/book", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = req.body.userId;
    const favoriteList = await FavoriteStore.findBookFavorite(userId);

    scope.setData(favoriteList);
    scope.setResult(true);
  });
});

export default router;
