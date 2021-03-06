#pragma once
#include <utility>
#include <memory>

namespace visit {
    namespace detail {

        template<typename VisitableBaseImpl, typename...Args>
        class VisitorVFunctions;

        template<typename VisitableBaseImpl, typename VisitedType>
        class VisitorVFunctions<VisitableBaseImpl, VisitedType> {
        protected:
            virtual void visit(VisitedType&) = 0;
        };

        template<typename VisitableBaseImpl, typename VisitedType, typename...Args>
        class VisitorVFunctions<VisitableBaseImpl, VisitedType, Args...>
            : public VisitorVFunctions<VisitableBaseImpl, Args...> {
        protected:
            using VisitorVFunctions<VisitableBaseImpl, Args...>::visit;
            virtual void visit(VisitedType&) = 0;
        };

        template<typename VisitableBaseImpl, typename...Args>
        class Visitor : public VisitorVFunctions<VisitableBaseImpl, Args...> {
        public:
            template<typename, typename> friend class Visitable;
            void visitPtr(std::unique_ptr<VisitableBaseImpl>& ptr) {
                ptr->accept(*this);
            }
        };

        template<typename VisitableBaseImpl, typename ReturnType, typename...Args>
        class VisitorReturner : public Visitor<VisitableBaseImpl, Args...> {
        public:
            template<typename, typename> friend class Visitable;

            ReturnType visitPtr(std::unique_ptr<VisitableBaseImpl>& visitable) {
                visitable->accept(*this);
                return flushRetval();
            }

            virtual ~VisitorReturner() {
                getPtr()->~ReturnType();
            }
        protected:
            template<typename U>
            void returnValue(U&& retval) {
                new(this->retval) ReturnType(std::forward<U>(retval));
            }
        private:
            alignas(ReturnType) char retval[sizeof(ReturnType)];

            ReturnType&& flushRetval() { return std::move(*getPtr()); }

            ReturnType* getPtr() {
                return static_cast<ReturnType*>(static_cast<void*>(&retval));
            }
        };

        template<typename VisitableBaseImpl, typename Derived>
        class Visitable : public VisitableBaseImpl {
        private:
            void accept(typename VisitableBaseImpl::VisitorType& visitor) override {
                visitor.visit(*static_cast<Derived*>(this));
            }
        };
    }
    
    template<typename Derived, typename...Args>
    class VisitableBase {
    public:
        template<typename ReturnType>
        using VisitorReturnerType = detail::VisitorReturner<Derived, ReturnType, Args...>;
        using VisitorType = detail::Visitor<Derived, Args...>;
        template<typename VisitableDerived>
        using Visitable = detail::Visitable<Derived, VisitableDerived>;
        virtual ~VisitableBase() = default;
    private:
        template<typename, typename...> friend class detail::Visitor;
        template<typename, typename, typename...> friend class detail::VisitorReturner;
        virtual void accept(VisitorType& visitor) = 0;
    };
}

