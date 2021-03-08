import Express from "express";
import { SupportQnaSchema } from "../core/repository";
import { SupportQnaDto } from "../core/store/SupportQnaStore";
import { SupportQnaStore, SupportQnaTypeStore } from "../core/store";
import { using } from "../core/util/Using";
import { ResponseScope } from "../Response";

const router = Express.Router();

type SignUpResult = {
  result: boolean;
  userId: number;
};

//support/qna/list
router.get("/qna/typelist", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const qnaTypeList = await SupportQnaTypeStore.findAll();

    scope.setData(qnaTypeList);
    scope.setResult(true);
  });
});

//support/qna/id
router.get("/qna/read/:id", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = parseInt(req.body.userId as string);
    const qnaData = await SupportQnaStore.findOne(
      parseInt(req.params.id),
      userId,
    );

    scope.setData(qnaData);
    scope.setResult(true);
  });
});

//support/qna/list
router.get("/qna/list", (req, resp) => {
  using(new ResponseScope(resp), async (scope) => {
    const userId = parseInt(req.body.userId as string);
    const qnaList = await SupportQnaStore.findAll(userId);

    scope.setData(qnaList);
    scope.setResult(true);
  });
});

var aws = require("aws-sdk");
var multer = require("multer");
var multerS3 = require("multer-s3");

var s3 = new aws.S3({
  accessKeyId: "AKIAIFICUWFTKCAS2D6Q",
  secretAccessKey: "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",
  region: "ap-northeast-2",
});

var upload = multer({
  storage: multerS3({
    s3: s3,
    bucket: "webtoonedu",
    acl: "public-read",
    contentType: multerS3.AUTO_CONTENT_TYPE,
    metadata: (req, file, cb) => {
      cb(null, { fieldName: file.fieldname });
    },
    key: (req, file, cb) => {
      cb(null, `static/qna/${Date.now().toString()}-${file.originalname}`);
    },
  }),
});

export interface MulterFile {
  key: string; // Available using `S3`.
  path: string; // Available using `DiskStorage`.
  mimetype: string;
  originalname: string;
  size: number;
  location: string;
}

export interface MetaData {
  fieldName: string;
}

export interface S3File {
  fieldname: string;
  originalname: string;
  encoding: string;
  mimetype: string;
  size: number;
  bucket: string;
  key: string;
  acl: string;
  contentType: string;
  contentDispositio: string;
  storageClass: string;
  serverSideEncryption: string;
  metadata: MetaData;
  location: string;
  etag: string;
}

export interface Response<T> {
  success: boolean;
  error: Error;
  data: T;
}

router.post(
  "/qna/write",
  (req, res, next) => {
    const userId = req.body.userId;
    upload.array("files")(req, res, () => {
      req.body.userId = userId;
      next();
    });
  },
  async (req: Express.Request & { files: MulterFile[] }, resp) => {
    using(new ResponseScope(resp), async (scope) => {
      const fileUrls: string[] = ["", "", ""];

      for (let index = 0; index < req.files.length; index++) {
        fileUrls[index] = req.files[index].key;
        fileUrls[index] = fileUrls[index].replace(/^(static\/)/, "");
      }

      const qnaData = new SupportQnaDto(<SupportQnaSchema>{
        id: req.body.id,
        userId: req.body.userId,
        regDate: new Date(),
        state: 0,
        qnaType: req.body.qnaType,
        title: req.body.title,
        questionText: req.body.description,
        file1: fileUrls[0],
        file2: fileUrls[1],
        file3: fileUrls[2],
        answerDate: null,
        answerText: "답변대기중",
      });
      const result = await SupportQnaStore.insert(qnaData);

      scope.setResult(true);
    });
  },
);

export default router;
