import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import { Typography, TypographyProps } from "@material-ui/core";

const useStyles = makeStyles((theme) => ({
  root: {
    fontSize: "1.25rem",
    [theme.breakpoints.down("xs")]: {
      fontSize: "1rem",
    },
  },
}));

interface Props extends TypographyProps {
  className?: string;
}

export const TypographyH2: React.FC<Props> = ({
  className,
  children,
  ...others
}: Props) => {
  const classes = useStyles();
  return (
    <Typography
      className={`${classes.root} ${className}`}
      variant="h1"
      {...others}
    >
      {children}
    </Typography>
  );
};
