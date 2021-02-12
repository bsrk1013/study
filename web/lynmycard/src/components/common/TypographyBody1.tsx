import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import { Typography, TypographyProps } from "@material-ui/core";

const useStyles = makeStyles((theme) => ({
  root: {
    fontSize: "16px",
    [theme.breakpoints.down("xs")]: {
      fontSize: "14px",
    },
    [theme.breakpoints.down(321)]: {
      fontSize: "12px",
    },
  },
}));

interface Props extends TypographyProps {
  className?: string;
}

export const TypographyBody1: React.FC<Props> = ({
  className,
  children,
  ...others
}: Props) => {
  const classes = useStyles();
  return (
    <Typography
      className={`${classes.root} ${className}`}
      variant="body1"
      {...others}
    >
      {children}
    </Typography>
  );
};
