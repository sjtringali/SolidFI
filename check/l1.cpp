#include "solidfi/solidfi.hpp"

using namespace solidfi;

struct Xml {};
struct Page {};

struct XmlToPage : Converter<Xml, Page> {
    Page resolve(Xml, solidfi::Parameters) override { return {}; }
};

struct Normalizer : Transform<Page> {
    Page apply(Page v, solidfi::Parameters) override { return v; }
};

static_assert(std::is_same_v<
    decltype(Deferred::create<XmlToPage>()),
    Deferred::Converter<Xml, Page>
>);

static_assert(std::is_same_v<
    decltype(Deferred::create<Normalizer>()),
    Deferred::Transform<Page>
>);

int main() { return 0; }
