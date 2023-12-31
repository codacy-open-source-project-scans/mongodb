import React from "react";
import { makeStyles } from "@material-ui/core/styles";
import Table from "@material-ui/core/Table";
import TableBody from "@material-ui/core/TableBody";
import TableCell from "@material-ui/core/TableCell";
import TableContainer from "@material-ui/core/TableContainer";
import TableHead from "@material-ui/core/TableHead";
import TableRow from "@material-ui/core/TableRow";
import Paper from "@material-ui/core/Paper";
import { connect } from "react-redux";
import { getCounts } from "./redux/store";
import { setCounts } from "./redux/counts";

const {REACT_APP_API_URL} = process.env;

const columns = [
  { id: "ID", field: "type", headerName: "Count Type", width: 50 },
  { field: "value", headerName: "Value", width: 50 },
];

const useStyles = makeStyles({
  table: {
    minWidth: 50,
  },
});

const GraphInfo = ({ selectedGraph, counts, datawidth, setCounts }) => {
  React.useEffect(() => {
    let gitHash = selectedGraph;
    if (gitHash) {
      fetch(REACT_APP_API_URL + '/api/graphs/' + gitHash + '/analysis')
        .then(response => response.json())
        .then(data => {
          setCounts(data.results);
        });
    }
  }, [selectedGraph]);
  
  const classes = useStyles();

  return (
    <TableContainer component={Paper}>
      <Table className={classes.table} size="small" aria-label="simple table">
        <TableHead>
          <TableRow>
            {columns.map((column, index) => {
              return <TableCell key={index}>{column.headerName}</TableCell>;
            })}
          </TableRow>
        </TableHead>
        <TableBody>
          {counts.map((row) => (
            <TableRow key={row.id}>
              <TableCell component="th" scope="row">
                {row.type}
              </TableCell>
              <TableCell>{row.value}</TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </TableContainer>
  );
};

export default connect(getCounts, { setCounts })(GraphInfo);
