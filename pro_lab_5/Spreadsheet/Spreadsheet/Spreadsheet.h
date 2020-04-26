#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <omp.h>

class Spreadsheet;
class SpreadsheetGenerator;
class SpreadsheetCalculator;


class Spreadsheet 
{
public:
	Spreadsheet(int numOfRows, int numOfCols);
	Spreadsheet();
	~Spreadsheet();

	int getRows() const;
	int getColumns() const;

	std::vector<std::string>& operator[](size_t index);
	const std::vector<std::string> operator[](size_t index) const;

	void resize(int numOfRows, int numOfCols);
	void print();
private:
	std::vector<std::vector<std::string>> cell;
};


class SpreadsheetGenerator
{
public:
	static Spreadsheet generate(const int numOfRows, const int numOfCols);
private:
	static std::string generateExpression();
protected:
	SpreadsheetGenerator();
};


class SpreadsheetCalculator
{
public:
	static std::string calculateExpression(const std::string expression);
	static Spreadsheet calculateSpreadsheet(const Spreadsheet spreadsheet);
protected:
	SpreadsheetCalculator();
};