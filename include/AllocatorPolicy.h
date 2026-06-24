#pragma once
#include <memory>

template<typename Alloc>
struct AllocatorPolicy {
    using allocator_type = Alloc;
    using traits_type = std::allocator_traits<allocator_type>;
    static constexpr bool copy_assign_propagates = traits_type::propagate_on_container_copy_assignment::value;
    static constexpr bool move_assign_propagates = traits_type::propagate_on_container_move_assignment::value;
    static constexpr bool swap_propagates = traits_type::propagate_on_container_swap::value;
    static constexpr bool always_equal = traits_type::is_always_equal::value;

    [[nodiscard]]
    static allocator_type
    select_copy_constructor_allocator(const allocator_type& alloc) {
        return traits_type::select_on_container_copy_construction(alloc);
    }

    [[nodiscard]]
    static constexpr bool
    allocators_compatible(const allocator_type& lhs, const allocator_type& rhs) noexcept(always_equal || noexcept(lhs == rhs))
    {
        if constexpr (always_equal) {
            return true;
        }
        return lhs == rhs;
    }
};
