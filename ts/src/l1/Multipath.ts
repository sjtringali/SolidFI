// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Converter } from './Converter';
import { Parameters } from './Parameters';
import { Chain } from './Chain';
import { Path } from './Path';

export class Multipath<T, U, P extends Parameters = Parameters> extends Path<T, U, P> {
    to<I>(converter: Converter<U, I, P>): Multipath<T, I, P> {
        return this.copyTo(new Multipath<T, I, P>()).addNode(converter) as Multipath<T, I, P>;
    }

    toEither<I>(...converters: Converter<U, I, P>[]): Multipath<T, I, P> {
        const chain = new Chain<U, I, P>();
        converters.forEach((c, i) => chain.install(i + 1, `option-${i + 1}`, c));
        return this.to(chain);
    }
}
