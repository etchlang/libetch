#ifndef ETCH_TRANSFORM_RESOLUTION_HPP
#define ETCH_TRANSFORM_RESOLUTION_HPP 1

#include <etch/transform/base.hpp>

namespace etch::transform {
	class resolution : public base {
	  public:
		ir::ptr<ir::base> visit(ir::ptr<ir::identifier> x) override {
			ir::ptr<ir::base> r = x;

			if(auto find = lookup(x->str)) {
				auto find_ty = find->type();
				auto fty = ir::as<ir::function>(find->type());

				if(ir::is<ir::type_type>(find_ty)) {
					r = find;
				} else if(fty && ir::is<ir::type_type>(fty->body)) {
					r = find;
				} else {
					x->resolve(find->type());
				}
			} else if(x->str == "+") {
				r = std::make_shared<ir::intr_add>();
			} else if(x->str == "*") {
				r = std::make_shared<ir::intr_mul>();
			}

			return r;
		}

		ir::ptr<ir::base> post(ir::ptr<ir::base> x) {
			if(ir::is<ir::type_unresolved>(x->type())) {
				std::ostringstream s;
				s << "transform::resolution: unresolved type: ";
				x->dump(s);
				auto str = s.str();

				std::cerr << str << std::endl << std::endl;
			}

			return x;
		}
	};
} // namespace etch::transform

#endif
