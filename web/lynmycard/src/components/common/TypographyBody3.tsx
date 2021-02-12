import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import { Typography, TypographyProps } from "@material-ui/core";

const useStyles = makeStyles((theme) => ({
  root: {
    fontSize: "20px",
    [theme.breakpoints.down("xs")]: {
      fontSize: "16px",
    },
    [theme.breakpoints.down(321)]: {
      fontSize: "15px",
    },
  },
}));

interface Props extends TypographyProps {
  className?: string;
}

export const TypographyBody3: React.FC<Props> = ({
  className,
  children,
  ...others
}: Props) => {
  const classes = useStyles();
  return (
    <Typography
      className={`${classes.root} ${className}`}
      variant="body2"
      {...others}
    >
      {children}
    </Typography>
  );
};
