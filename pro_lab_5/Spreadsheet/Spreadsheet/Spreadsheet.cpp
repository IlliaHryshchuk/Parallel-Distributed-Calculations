#include "Spreadsheet.h"



Spreadsheet::Spreadsheet(){}
Spreadsheet::Spreadsheet(int numOfRows, int numOfCols){	this->resize(numOfRows, numOfCols); }
Spreadsheet::~Spreadsheet() {}

int Spreadsheet::getRows() const
{
	return  this->cell.size();
}
int Spreadsheet::getColumns() const
{
	return  cell.empty() ? 0 : cell.front().size();
}

std::vector<std::string>& Spreadsheet::operator[](size_t index)
{
	return cell[index];
}
const std::vector<std::string> Spreadsheet::operator[](size_t index) const
{
	return cell[index];
}

void Spreadsheet::resize(int numOfRows, int numOfCols)
{
	cell.resize(numOfRows);
	for (int i = 0; i < numOfRows; i++)
		cell[i].resize(numOfCols);
}

void Spreadsheet::print()
{
	for (int i = 0; i < this->getRows(); i++)
	{
		for (int j = 0; j < this->getColumns(); j++)
			std::cout << this->cell[i][j] << "\t";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}




SpreadsheetGenerator::SpreadsheetGenerator() {}

std::string SpreadsheetGenerator::generateExpression()
{
	int numOfExpr, valA, valB;
	
	numOfExpr = rand() % 4 + 1;
	valA = rand() % 50 + 1;
	valB = rand() % 50 + 1;

	switch (numOfExpr)
	{
	case 1:
		return std::to_string(valA) + "+" + std::to_string(valB);
	case 2:
		return std::to_string(valA) + "-" + std::to_string(valB);
	case 3:
		return std::to_string(valA) + "*" + std::to_string(valB);
	case 4:
		return std::to_string(valA) + "/" + std::to_string(valB);
	}
}

Spreadsheet SpreadsheetGenerator::generate(const int numOfRows, const int numOfCols)
{

	Spreadsheet spreadsheet;
	spreadsheet.resize(numOfRows, numOfCols);

#pragma omp parallel
	{
#pragma omp for 
		for (int i = 0; i < numOfRows; i++)
		{
			for (int j = 0; j < numOfCols; j++)
			{
				spreadsheet[i][j] = generateExpression();
			}
		}
	}
	return spreadsheet;
}




SpreadsheetCalculator::SpreadsheetCalculator() {}

std::string SpreadsheetCalculator::calculateExpression(const std::string expression)
{
	std::string leftOperand, rightOperand, operation;
	float result;

	for (int i = 0; i < expression.length(); i++) {
		if (isdigit(expression[i])) {
			if (operation.empty()) {
				leftOperand += expression[i];
				continue;
			}
			rightOperand += expression[i];
			continue;
		}
		operation = expression[i];
	}

	if (operation == "+") result = stof(leftOperand) + stof(rightOperand);
	else if (operation == "-") result = stof(leftOperand) - stof(rightOperand);
	else if (operation == "/") result = stof(leftOperand) / stof(rightOperand);
	else if (operation == "*") result = stof(leftOperand) * stof(rightOperand);
	else return "error";

	return std::to_string(result).substr(0, std::to_string(result).find('.')+3);
}

Spreadsheet SpreadsheetCalculator::calculateSpreadsheet(const Spreadsheet spreadsheet)
{
	Spreadsheet spreadsheet_with_answers(spreadsheet.getRows(), spreadsheet.getColumns());
	std::string expression;

#pragma omp parallel
	{
#pragma omp for 
		for (int i = 0; i < spreadsheet.getRows(); i++)
		{
			for (int j = 0; j < spreadsheet.getColumns(); j++)
				spreadsheet_with_answers[i][j] = SpreadsheetCalculator::calculateExpression(spreadsheet[i][j]);
		}
	}
	return spreadsheet_with_answers;
}