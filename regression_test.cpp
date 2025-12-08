/*
  Compile with:
     c++ -pedantic-errors -Wall -Wextra -Werror -Wno-long-long -g -O0 -o regression_test regression_test.cpp \
          -L/usr/lib -lstdc++ -lm
*/

#include "exprtk.hpp"

#include <iostream>
#include <string>

int main() {
  const std::string expression_string = "return [x + 10]";

  exprtk::symbol_table<double> symbol_table;
  exprtk::expression<double> expression;
  exprtk::parser<double> parser;

  std::vector<double> temp(1);
  temp[0] = 43;
  exprtk::vector_view<double> x_view = exprtk::make_vector_view(temp, temp.size());

  symbol_table.add_vector("x", x_view);

  expression.register_symbol_table(symbol_table);

  bool success = parser.compile(expression_string, expression);
  assert(success);

  std::vector<double> x(1);
  x[0] = 42;

  // Expected result with the following line: 42 + 10 = 52
  // Expected result with the following line commented: 43 + 10 = 53
  x_view.rebase(x.data());

  expression.value();

  assert(expression.return_invoked());

  const exprtk::results_context<double>& results = expression.results();

  assert(results.count() == 1);

  assert(results[0].type == exprtk::results_context<double>::type_store_t::e_vector);

  // create view for result
  exprtk::type_store<double>::type_view<double> rv(results[0]);

  // This can also be used in version 0.0.3 with the same result:
  // std::vector<double> rv;
  // results.get_vector(0, rv);

  assert(rv.size() == 1);

  // Actual result with the above rebase() call enabled: 42 (value assigned to x[0] above)
  // Actual result with the above rebase() call disabled: 53 (correct)
  std::cout << "Result: " << rv[0] << std::endl;

  return 0;
}
