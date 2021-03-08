import Express from "express";
import BookEpisodeStore from "../core/store/EpisodeStore";
import EpisodeStore from "../core/store/EpisodeStore";
import { EpisodeSchema } from "../core/repository";
import ProductStore, { SubscriptionDto } from "../core/store/ProductStore";
import UserStore from "../core/store/UserStore";
import { using } from "../core/util/Using";
import { ResponseScope } from "../Response";
const router = Express.Router();

router.get("/list", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const bookId = parseInt(req.query.bookId as string);
    const episode = await BookEpisodeStore.getShowingEpisodesByBook(bookId);

    scope.setData(episode);
    scope.setResult(true);
  });
});

router.post("/log", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const bookId: number = req.body.bookId;
    const episodeId: number = req.body.episodeId;
    const userId: number = req.body.userId;
    const publicIp: string = req.body.publicIp;

    // 책 검증 필요??
    // const bookInfo = await BookStore.findOne(bookId);
    // const episodeInfo = await EpisodeStore.findOne(episodeId);

    if (userId && userId > 0) {
      const userInfo = await UserStore.findOne(userId);
      if (!userInfo) {
        return;
      }

      const exists = await EpisodeStore.existsPageViewLog(
        bookId,
        episodeId,
        userId,
      );
      if (!exists) {
        await EpisodeStore.emitPageViewLog(bookId, episodeId, userId);
      }

      // 상세 기록 작성
      let subscriptionInfo: SubscriptionDto | null = null;
      if (userInfo.isSubscription() && userInfo.subscriptionPid > 0) {
        subscriptionInfo = await ProductStore.findOneBySubscription(
          userInfo.subscriptionPid,
        );
      }

      // 만료시간 지난 로그 삭제 코드
      // await EpisodeStore.deleteViewDetailLog(userId);

      await EpisodeStore.emitViewDetailLog(
        userId,
        publicIp,
        bookId,
        episodeId,
        subscriptionInfo ? subscriptionInfo.id : 0,
        subscriptionInfo ? subscriptionInfo.name : "비구독",
      );
    } else {
      // 비회원 로그는 따로 요청이 없을시 남기지 않는다.
      // await EpisodeStore.emitViewDetailLog(
      //   0,
      //   publicIp,
      //   bookId,
      //   bookInfo.title,
      //   episodeId,
      //   episodeInfo.title,
      //   0,
      //   "비회원",
      // );
    }

    scope.setResult(true);
  });
});

router.get("/viewdetaillog", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = req.body.userId;
    const page: number = Number.parseInt(req.query.page as string);
    const result = await EpisodeStore.findAllViewDetailLog(userId, page);

    scope.setData(result);
    scope.setResult(true);
  });
});

router.get("/:id/detail", async (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const id = parseInt(req.params.id as string);
    const episode = await EpisodeStore.getEpisodeDetail(id);

    scope.setData(episode);
    scope.setResult(true);
  });
});

router.get("/:id/contents/list", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const id = parseInt(req.params.id);
    const list = await EpisodeStore.getContents(id);

    scope.setData(list);
    scope.setResult(true);
  });
});

router.get("/:id", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const id = parseInt(req.params.id as string);
    const episode = await EpisodeStore.findOne(id);

    scope.setData(episode);
    scope.setResult(true);
  });
});

router.post("/", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    await EpisodeStore.insert(<EpisodeSchema>{
      bookId: req.body.bookId,
      title: req.body.title,
      episodeNo: req.body.episodeNo,
      description: req.body.description,
      isHide: req.body.isHide,
      availableDate: req.body.availableDate,
    });

    scope.setResult(true);
  });
});

export default router;
