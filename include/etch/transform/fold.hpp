#ifndef ETCH_TRANSFORM_FOLD_HPP
#define ETCH_TRANSFORM_FOLD_HPP 1

#include <etch/transform/base.hpp>

namespace etch::transform {
	class fold : public base {
	  public:
		ir::ptr<ir::base> visit(ir::ptr<ir::call> x) override {
			ir::ptr<ir::base> r = x;

			auto t = ir::as<ir::tuple>(x->arg);

			if(auto intr = ir::as<ir::intr_int>(x->fn)) {
				auto c = ir::as<ir::constant_int>(x->arg);
				if(c) {
					r = std::make_shared<ir::type_int>(c->val);
				}
			} else if(auto intr = ir::as<ir::intr_add>(x->fn)) {
				auto lhs = ir::as<ir::constant_int>(t->vals[0]);
				auto rhs = ir::as<ir::constant_int>(t->vals[1]);
				if(lhs && rhs) {
					r = std::make_shared<ir::constant_int>(lhs->val + rhs->val);
				}
			} else if(auto intr = ir::as<ir::intr_mul>(x->fn)) {
				auto lhs = ir::as<ir::constant_int>(t->vals[0]);
				auto rhs = ir::as<ir::constant_int>(t->vals[1]);
				if(lhs && rhs) {
					r = std::make_shared<ir::constant_int>(lhs->val * rhs->val);
				}
			}

			return r;
		}

		ir::ptr<ir::base> visit(ir::ptr<ir::tuple> x) override {
			ir::ptr<ir::base> r = x;

			auto result = std::make_shared<ir::tuple>();

			for(auto &val : x->vals) {
				auto new_val = run(val);
				auto new_tuple = ir::as<ir::tuple>(new_val);
				if(!new_tuple || new_tuple->vals.size() > 0) {
					result->push_back(new_val);
				}
			}

			if(result->vals.size() == 1) {
				r = result->vals[0];
			} else {
				r = result;
			}

			return r;
		}

		ir::ptr<ir::base> visit(ir::ptr<ir::block> x) override {
			ir::ptr<ir::base> r = x;

			if(x->vals.size() == 1) {
				r = x->vals[0];
			}

			return r;
		}

		ir::ptr<ir::base> visit(ir::ptr<ir::cast> x) override {
			ir::ptr<ir::base> r = x;

			if(auto ty_int = ir::as<ir::type_int>(x->ty)) {
				if(auto val_int = ir::as<ir::constant_int>(x->val)) {
					r = std::make_shared<ir::constant_int>(val_int->val, ty_int->width);
				}
			}

			return r;
		}
	};
} // namespace etch::transform

#endif
