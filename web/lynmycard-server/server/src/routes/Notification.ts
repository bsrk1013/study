import Express from "express";
import { EpisodeStore, FavoriteStore } from "../core/store";
import { EpisodeDto } from "../core/store/EpisodeStore";
import BookStore, { BookDto } from "../core/store/BookStore";
import { using } from "../core/util/Using";
import { ResponseScope } from "../Response";

const router = Express.Router();

router.get("/", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = req.body.userId;

    let unreadEpisodeList: EpisodeDto[] = [];
    let unreadBookList: BookDto[] = [];

    if (userId) {
      const favoriteList = await FavoriteStore.findBookFavorite(userId);
      if (favoriteList.length > 0) {
        const viewLogList = await EpisodeStore.findAllViewLogByBookIds(
          userId,
          favoriteList,
        );

        const episodeList = await EpisodeStore.getEpisodesByBooks(favoriteList);
        unreadEpisodeList = episodeList.filter(
          (c) => !viewLogList.find((d) => d.episodeId === c.id),
        );

        const bookList = await BookStore.findById(favoriteList);
        unreadBookList = bookList.filter((c) =>
          unreadEpisodeList.find((d) => d.bookId === c.id),
        );
      }
    }

    scope.setData({ unreadEpisodeList, unreadBookList });
    scope.setResult(true);
  });
});

export default router;
