// #include "mc_collision_group.h"
// #include "mc_signal.h"
// #include "mc_bind.h"
// #include <boost/multi_index_container.hpp>
// #include <boost/multi_index/ordered_index.hpp>
// #include <boost/multi_index/hashed_index.hpp>
// #include <boost/multi_index/member.hpp>
// #include <boost/multi_index/composite_key.hpp>
// namespace
// {
// struct BodyRecord
// {
//   dynamics::Body::WeakPtr body_;
//   std::string group_;

//   BodyRecord(dynamics::Body::WeakPtr const& body, std::string const& group) : body_(body), group_(group)
//   {
//   }
// };

// typedef boost::multi_index_container
// <
//   BodyRecord,
//   boost::multi_index::indexed_by
//   <
//     boost::multi_index::ordered_unique
//     <
//       boost::multi_index::composite_key
//       <
//         BodyRecord,
//         boost::multi_index::member<BodyRecord, dynamics::Body::WeakPtr, &BodyRecord::body_>,
//         boost::multi_index::member<BodyRecord, std::string, &BodyRecord::group_>
//       >
//     >
//   >
// > BodyMap;

// struct GroupRecord
// {
//   std::string group_a_;
//   std::string group_b_;
//   event::Signal begin_;
//   event::Signal end_;
//   GroupRecord(std::string const& group_a, std::string const& group_b) : group_a_(group_a), group_b_(group_b) 
//   {
//   }
// };

// typedef boost::multi_index_container
// <
//   GroupRecord,
//   boost::multi_index::indexed_by
//   <
//     boost::multi_index::hashed_unique
//     <
//       boost::multi_index::composite_key
//       <
//         GroupRecord,
//         boost::multi_index::member<GroupRecord, std::string, &GroupRecord::group_a_>,
//         boost::multi_index::member<GroupRecord, std::string, &GroupRecord::group_b_>
//       >
//     >,
//     boost::multi_index::hashed_non_unique
//     <
//       boost::multi_index::member<GroupRecord, std::string, &GroupRecord::group_a_>
//     >,
//     boost::multi_index::hashed_non_unique
//     <
//       boost::multi_index::member<GroupRecord, std::string, &GroupRecord::group_b_>
//     >
//   >
// > GroupMap;

// auto Find(GroupMap const& collisions, std::string const& group_a, std::string const& group_b) -> GroupMap::nth_index<0>::type::iterator
// {
//   if(group_a < group_b)
//   {
//     return collisions.find(boost::make_tuple(group_a, group_b));
//   }
//   else
//   {
//     return collisions.find(boost::make_tuple(group_b, group_a));
//   }
// }
// }

// namespace collision
// {
// class Group::Impl
// {
// public:
//   Impl(event::Queue& queue);
//   auto Begin(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void;
//   auto End(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void;
//   auto Link(std::string const& group_a, dynamics::Body const& body_a) -> void;
//   auto Unlink(std::string const& group_a, dynamics::Body const& body_a) -> void;
//   auto Link(std::string const& group_a, std::string const& group_b) -> void;
//   auto Unlink(std::string const& group_a, std::string const& group_b) -> void;
//   auto Check(dynamics::Body const& body_a, dynamics::Body const& body_b) -> bool;
//   auto Begin(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void;
//   auto End(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void;
//   auto Unlink(dynamics::Body const& body_a) -> void;
//   auto Unlink(std::string const& group_a) -> void;
//   BodyMap members_;
//   GroupMap collisions_;
//   event::Queue queue_;
// };

// Group::Impl::Impl(event::Queue& queue) : queue_(queue)
// {
// }

// auto Group::Impl::Begin(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
// {
//   auto iter = Find(collisions_, group_a, group_b);
//   if(iter != collisions_.end())
//   {
//     collisions_.modify(iter, [&](GroupRecord& record){record.begin_.Add(command);});
//   }
// }

// auto Group::Impl::End(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
// {
//   auto iter = Find(collisions_, group_a, group_b);
//   if(iter != collisions_.end())
//   {
//     collisions_.modify(iter, [&](GroupRecord& record){record.end_.Add(command);});
//   }
// }

// auto Group::Impl::Link(std::string const& group_a, dynamics::Body const& body_a) -> void
// {
//   members_.emplace(body_a, group_a);
// }

// auto Group::Impl::Unlink(std::string const& group_a, dynamics::Body const& body_a) -> void
// {
//   auto iter = members_.find(boost::make_tuple(body_a, group_a));
//   if(iter != members_.end())
//   {
//     members_.erase(iter);
//   }
// }

// auto Group::Impl::Link(std::string const& group_a, std::string const& group_b) -> void
// {
//   if(group_a < group_b)
//   {
//     collisions_.emplace(group_a, group_b);
//   }
//   else
//   {
//     collisions_.emplace(group_b, group_a);
//   }
// }

// auto Group::Impl::Unlink(std::string const& group_a, std::string const& group_b) -> void
// {
//   auto iter = Find(collisions_, group_a, group_b);
//   if(iter != collisions_.end())
//   {
//     collisions_.erase(iter);
//   }
// }

// auto Group::Impl::Check(dynamics::Body const& body_a, dynamics::Body const& body_b) -> bool
// {
//   auto range_a = members_.equal_range(body_a);
//   auto range_b = members_.equal_range(body_b);
//   for(auto j = range_b.first; j != range_b.second; ++j)
//   {
//     for(auto i = range_a.first; i != range_a.second; ++i)
//     {
//       auto iter = Find(collisions_, i->group_, j->group_);
//       if(iter != collisions_.end())
//       {
//         return true;
//       }
//     }
//   }
//   return false;
// }

// auto Group::Impl::Begin(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void
// {
//   auto range_a = members_.equal_range(body_a);
//   auto range_b = members_.equal_range(body_b);
//   for(auto j = range_b.first; j != range_b.second; ++j)
//   {
//     for(auto i = range_a.first; i != range_a.second; ++i)
//     {
//       auto iter = Find(collisions_, i->group_, j->group_);
//       if(iter != collisions_.end())
//       {
//         collisions_.modify(iter, [&](GroupRecord& record){record.begin_(queue_);});
//       }
//     }
//   }
// }

// auto Group::Impl::End(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void
// {
//   auto range_a = members_.equal_range(body_a);
//   auto range_b = members_.equal_range(body_b);
//   for(auto j = range_b.first; j != range_b.second; ++j)
//   {
//     for(auto i = range_a.first; i != range_a.second; ++i)
//     {
//       auto iter = Find(collisions_, i->group_, j->group_);
//       if(iter != collisions_.end())
//       {
//         collisions_.modify(iter, [&](GroupRecord& record){record.end_(queue_);});
//       }
//     }
//   }
// }

// auto Group::Impl::Unlink(dynamics::Body const& body_a) -> void
// {
//   auto range_a = members_.equal_range(body_a);
//   members_.erase(range_a.first, range_a.second);
// }

// auto Group::Impl::Unlink(std::string const& group_a) -> void
// {
//   collisions_.get<1>().erase(group_a);
//   collisions_.get<2>().erase(group_a);
// }

// Group::Group(event::Queue& queue) : impl_(std::make_shared<Impl>(queue))
// {
// }

// auto Group::Begin(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
// {
//   impl_->Begin(group_a, group_b, command);
// }

// auto Group::End(std::string const& group_a, std::string const& group_b, event::Command const& command) -> void
// {
//   impl_->End(group_a, group_b, command);
// }

// auto Group::Link(std::string const& group_a, dynamics::Body const& body_a) -> void
// {
//   impl_->Link(group_a, body_a);
// }

// auto Group::Unlink(std::string const& group_a, dynamics::Body const& body_a) -> void
// {
//   impl_->Unlink(group_a, body_a);
// }

// auto Group::Link(std::string const& group_a, std::string const& group_b) -> void
// {
//   impl_->Link(group_a, group_b);
// }

// auto Group::Unlink(std::string const& group_a, std::string const& group_b) -> void
// {
//   impl_->Unlink(group_a, group_b);
// }

// auto Group::Check(dynamics::Body const& body_a, dynamics::Body const& body_b) -> bool
// {
//   return impl_->Check(body_a, body_b);
// }

// auto Group::Begin(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void
// {
//   impl_->Begin(body_a, body_b);
// }

// auto Group::End(dynamics::Body const& body_a, dynamics::Body const& body_b) -> void
// {
//   impl_->End(body_a, body_b);
// }

// auto Group::Unlink(dynamics::Body const& body_a) -> void
// {
//   impl_->Unlink(body_a);
// }

// auto Group::Unlink(std::string const& group_a) -> void
// {
//   impl_->Unlink(group_a);
// }
// }

#include "mc_collision_group.h"
#include "mc_signal.h"
#include "mc_bind.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <algorithm>

namespace {
// 组合键结构，保证 group_a <= group_b
struct GroupKey {
    std::string a;
    std::string b;

    GroupKey(std::string x, std::string y) {
        if (x < y) { a = x; b = y; } 
        else       { a = y; b = x; }
    }

    bool operator==(const GroupKey& o) const {
        return a == o.a && b == o.b;
    }
};

// 哈希函数
struct GroupKeyHash {
    size_t operator()(const GroupKey& k) const {
        return std::hash<std::string>{}(k.a) ^ 
              (std::hash<std::string>{}(k.b) << 1);
    }
};

// Body 到分组的映射（使用原始指针 + 弱引用）
struct BodyRecord {
    dynamics::Body::WeakPtr body;
    std::string group;

    BodyRecord(dynamics::Body::WeakPtr b, std::string g)
        : body(std::move(b)), group(std::move(g)) {}
};

// Body 映射比较函数
struct BodyCompare {
    bool operator()(const dynamics::Body* a, const dynamics::Body* b) const {
        return a < b;
    }
};

// 主数据结构
std::multimap<dynamics::Body*, std::string, BodyCompare> body_mappings; // Body 映射
std::unordered_map<GroupKey, GroupRecord, GroupKeyHash> group_pairs;    // 碰撞分组对
std::unordered_multimap<std::string, GroupKey> group_a_index;           // 分组A索引
std::unordered_multimap<std::string, GroupKey> group_b_index;           // 分组B索引

// 查找分组对（自动处理顺序）
auto FindGroupPair(const std::string& ga, const std::string& gb) {
    return group_pairs.find(GroupKey(ga, gb));
}

// 清理反向索引
void CleanIndex(GroupKey key) {
    // 清理 group_a 索引
    auto range_a = group_a_index.equal_range(key.a);
    for (auto it = range_a.first; it != range_a.second;) {
        it->second == key ? it = group_a_index.erase(it) : ++it;
    }

    // 清理 group_b 索引
    auto range_b = group_b_index.equal_range(key.b);
    for (auto it = range_b.first; it != range_b.second;) {
        it->second == key ? it = group_b_index.erase(it) : ++it;
    }
}
} // namespace

namespace collision {
class Group::Impl {
public:
    Impl(event::Queue& queue) : event_queue(queue) {}

    // 事件绑定
    void Begin(const std::string& ga, const std::string& gb, event::Command cmd) {
        if (auto it = FindGroupPair(ga, gb); it != group_pairs.end()) {
            it->second.begin_.Add(std::move(cmd));
        }
    }

    void End(const std::string& ga, const std::string& gb, event::Command cmd) {
        if (auto it = FindGroupPair(ga, gb); it != group_pairs.end()) {
            it->second.end_.Add(std::move(cmd));
        }
    }

    // Body 关联
    void LinkBody(const std::string& group, const dynamics::Body& body) {
        if (auto sp = body.weak_from_this().lock()) {
            body_mappings.emplace(sp.get(), group);
        }
    }

    void UnlinkBody(const std::string& group, const dynamics::Body& body) {
        if (auto sp = body.weak_from_this().lock()) {
            auto range = body_mappings.equal_range(sp.get());
            for (auto it = range.first; it != range.second;) {
                it->second == group ? it = body_mappings.erase(it) : ++it;
            }
        }
    }

    // 分组关联
    void LinkGroups(const std::string& ga, const std::string& gb) {
        GroupKey key(ga, gb);
        if (auto [it, inserted] = group_pairs.try_emplace(key, ga, gb); inserted) {
            group_a_index.emplace(ga, key);
            group_b_index.emplace(gb, key);
        }
    }

    void UnlinkGroups(const std::string& ga, const std::string& gb) {
        if (auto it = FindGroupPair(ga, gb); it != group_pairs.end()) {
            CleanIndex(it->first);
            group_pairs.erase(it);
        }
    }

    // 碰撞检测
    bool CheckCollision(const dynamics::Body& a, const dynamics::Body& b) const {
        auto get_groups = [](const dynamics::Body* body) {
            std::vector<std::string> groups;
            auto range = body_mappings.equal_range(body);
            for (auto it = range.first; it != range.second; ++it) {
                groups.push_back(it->second);
            }
            return groups;
        };

        auto a_groups = get_groups(&a);
        auto b_groups = get_groups(&b);

        for (const auto& ga : a_groups) {
            for (const auto& gb : b_groups) {
                if (group_pairs.count(GroupKey(ga, gb))) return true;
            }
        }
        return false;
    }

    // 触发事件
    void TriggerBegin(const dynamics::Body& a, const dynamics::Body& b) {
        TriggerEvent(a, b, [](GroupRecord& r) { r.begin_.Emit(); });
    }

    void TriggerEnd(const dynamics::Body& a, const dynamics::Body& b) {
        TriggerEvent(a, b, [](GroupRecord& r) { r.end_.Emit(); });
    }

    // 清理操作
    void ClearBody(const dynamics::Body& body) {
        if (auto sp = body.weak_from_this().lock()) {
            body_mappings.erase(sp.get());
        }
    }

    void ClearGroup(const std::string& group) {
        // 清理正向索引
        std::vector<GroupKey> to_erase;
        auto scan_index = [&](auto& index) {
            for (auto it = index.begin(); it != index.end();) {
                if (it->first == group) {
                    to_erase.push_back(it->second);
                    it = index.erase(it);
                } else {
                    ++it;
                }
            }
        };

        scan_index(group_a_index);
        scan_index(group_b_index);

        // 去重后删除主记录
        std::sort(to_erase.begin(), to_erase.end());
        auto last = std::unique(to_erase.begin(), to_erase.end());
        for (auto it = to_erase.begin(); it != last; ++it) {
            group_pairs.erase(*it);
        }
    }

private:
    template <typename F>
    void TriggerEvent(const dynamics::Body& a, const dynamics::Body& b, F&& handler) {
        auto get_groups = [](const dynamics::Body* body) {
            std::vector<std::string> groups;
            auto range = body_mappings.equal_range(body);
            for (auto it = range.first; it != range.second; ++it) {
                groups.push_back(it->second);
            }
            return groups;
        };

        auto a_groups = get_groups(&a);
        auto b_groups = get_groups(&b);

        for (const auto& ga : a_groups) {
            for (const auto& gb : b_groups) {
                if (auto it = FindGroupPair(ga, gb); it != group_pairs.end()) {
                    handler(it->second);
                }
            }
        }
    }

    event::Queue& event_queue;
};

// 接口实现（保持与原始代码一致）
Group::Group(event::Queue& q) : impl(std::make_unique<Impl>(q)) {}
void Group::Begin(const std::string& a, const std::string& b, event::Command c) { impl->Begin(a, b, c); }
void Group::End(const std::string& a, const std::string& b, event::Command c) { impl->End(a, b, c); }
void Group::LinkBody(const std::string& g, const dynamics::Body& b) { impl->LinkBody(g, b); }
void Group::UnlinkBody(const std::string& g, const dynamics::Body& b) { impl->UnlinkBody(g, b); }
void Group::LinkGroups(const std::string& a, const std::string& b) { impl->LinkGroups(a, b); }
void Group::UnlinkGroups(const std::string& a, const std::string& b) { impl->UnlinkGroups(a, b); }
bool Group::CheckCollision(const dynamics::Body& a, const dynamics::Body& b) const { return impl->CheckCollision(a, b); }
void Group::TriggerBegin(const dynamics::Body& a, const dynamics::Body& b) { impl->TriggerBegin(a, b); }
void Group::TriggerEnd(const dynamics::Body& a, const dynamics::Body& b) { impl->TriggerEnd(a, b); }
void Group::ClearBody(const dynamics::Body& b) { impl->ClearBody(b); }
void Group::ClearGroup(const std::string& g) { impl->ClearGroup(g); }
} // namespace collision
