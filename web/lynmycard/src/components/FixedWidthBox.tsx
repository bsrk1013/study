import React from "react";
import * as mui from "@material-ui/core";

const useStyles = mui.makeStyles((theme) => ({
  root: {
    maxWidth: "800px",
    margin: "0 auto",
    boxSizing: "border-box",
    paddingTop: "128px",
    [theme.breakpoints.down("xs")]: {
      paddingTop: "95px",
    },
  },
}));

interface Props extends mui.BoxProps {
  pt?: number;
}

const FixedWidthBox: React.FunctionComponent<Props> = ({
  children,
  className,
  pt,
  ...others
}: React.PropsWithChildren<Props>) => {
  const classes = useStyles();
  return (
    <mui.Box className={classes.root}>
      <mui.Box pt={pt} {...others} className={`${className}`}>
        {children}
      </mui.Box>
    </mui.Box>
  );
};

export default FixedWidthBox;
