import Express from "express";
import { BannerStore } from "../core/store";
import { ResponseScope } from "../Response";
import { using } from "../core/util/Using";

const router = Express.Router();

router.get("/", (req, resp, next) => {
  using(new ResponseScope(resp), async (scope) => {
    const bundleKey = Number.parseInt(req.query.bundle_key as string);
    const bannerList = await BannerStore.find(bundleKey);

    scope.setData(bannerList);
    scope.setResult(true);
  });
});

export default router;
