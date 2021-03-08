import Express from "express";
import AuthorStore from "../core/store/AuthorStore";
import { using } from "../core/util/Using";
import { ResponseScope } from "../Response";
const router = Express.Router();

router.get("/list", async (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    scope.setData(await AuthorStore.findAll());
    scope.setResult(true);
  });
});

router.get("/:id", async (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const id = parseInt(req.params.id);
    const author = await AuthorStore.findOne(id);

    scope.setData(author);
    scope.setResult(true);
  });
});

router.post("/", async (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const name: string = req.body.name;
    await AuthorStore.insert(name);

    scope.setResult(true);
  });
});

export default router;
