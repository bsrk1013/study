import "./styles/styles.css";
import * as React from "react";
import * as mui from "@material-ui/core";
import { TypographyH1 } from "./components/common/TypographyH1";
import { TypographyH2 } from "./components/common/TypographyH2";
import * as products from "./products";
import axios, { AxiosRequestConfig } from "axios";
import { Response, responseReturn } from "./Response";

const useStyles = mui.makeStyles((theme: mui.Theme) =>
  mui.createStyles({
    root: {
      flexGrow: 1,
    },
    paper: {
      width: "100%",
      height: "auto",
      display: "flex",
      flexDirection: "column",
      alignItems: "center",
      padding: "15px",
    },
    bold: {
      fontWeight: "bold",
    },
    divider: {
      width: "95%",
      height: "1px",
      margin: "40px 0px",
    },
    idPaper: {
      display: "flex",
      justifyContent: "space-between",
      width: "100%",
    },
    idBox: {
      width: "100%",
      margin: "0px 20px",
    },
    selectBox: {
      width: "80%",
      margin: "20px 20px",
    },
    button: {
      width: "80%",
      margin: "30px 20px",
    },
  }),
);

export default function App() {
  const classes = useStyles();

  const [state, setState] = React.useState<{
    product: string | number;
    name: string;
  }>({
    product: "",
    name: "hai",
  });

  const [errorState, setErrorState] = React.useState<{
    idError: string;
  }>({ idError: "" });

  React.useEffect(() => {
    init();
  }, []);

  function init() {
    console.log(products.default.products);
  }

  function renderSelectOption() {
    return products.default.products.map((product) => {
      return <option value={product.code}>{product.english}</option>;
    });
  }

  async function onBuy() {
    const id = document.getElementById("uniqueId") as HTMLInputElement;
    const reid = document.getElementById("reuniqueId") as HTMLInputElement;

    const strId = id.value;
    const strReId = reid.value;

    var result: boolean = strId.localeCompare(strReId) === 0;
    if (result) {
      const checkResp = await axios.get<Response<{ result: number }>>(
        "http://127.0.0.7:4100/mycard/checkAccount",
        {
          params: {
            userId: strId,
          },
        },
      );

      result = (checkResp.data as any).result === 1;
    }

    if (!result) {
      setErrorState({
        ...errorState,
        idError: "check unique id",
      });
      id.focus();
      return;
    }

    setErrorState({
      ...errorState,
      idError: "",
    });
  }

  const handleChange = (
    event: React.ChangeEvent<{ name?: string; value: unknown }>,
  ) => {
    const name = event.target.name as keyof typeof state;
    setState({
      ...state,
      [name]: event.target.value,
    });

    const data = products.default.products.find(
      (c) => c.code == event.target.value,
    );
  };

  return (
    <mui.Box className={classes.root}>
      <mui.Paper className={classes.paper}>
        <TypographyH1 className={classes.bold}>CASH SHOP</TypographyH1>
        <mui.Divider className={classes.divider} />
        <mui.Box className={classes.idPaper}>
          <mui.TextField
            id="uniqueId"
            label="Unique ID"
            variant="outlined"
            error={errorState.idError.length > 0}
            helperText={errorState.idError}
            className={classes.idBox}
          />
          <mui.TextField
            id="reuniqueId"
            label="Confirm Unique ID"
            variant="outlined"
            className={classes.idBox}
          />
        </mui.Box>
        <mui.Divider className={classes.divider} />
        <TypographyH2 className={classes.bold}>PRODUCT</TypographyH2>
        <mui.FormControl variant="outlined" className={classes.selectBox}>
          <mui.InputLabel htmlFor="outlined-product-native-simple">
            Product
          </mui.InputLabel>
          <mui.Select
            native
            value={state.product}
            onChange={handleChange}
            label="Product"
            inputProps={{
              name: "product",
              id: "outlined-product-native-simple",
            }}
          >
            <option aria-label="None" value="" />
            {renderSelectOption()}
          </mui.Select>
        </mui.FormControl>
        <mui.Button
          variant="contained"
          color="primary"
          className={classes.button}
          onClick={onBuy}
        >
          Buy
        </mui.Button>
      </mui.Paper>
    </mui.Box>
  );
}
