import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import { Typography, TypographyProps } from "@material-ui/core";

const useStyles = makeStyles((theme) => ({
  root: {
    fontSize: "15px",
    [theme.breakpoints.down(600)]: {
      fontSize: "13px",
    },
    [theme.breakpoints.down(400)]: {
      fontSize: "10px",
    },
  },
}));

interface Props extends TypographyProps {
  className?: string;
}

export const TypographyBody2: React.FC<Props> = ({
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
