// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Converter } from './Converter';
import { Parameters } from './Parameters';
import { Transform } from './Transform';

export abstract class Provider<T, U, P extends Parameters = Parameters>
    implements Converter<T, U, P> {
    prepare?: Transform<T, P>;
    finalize?: Transform<U, P>;

    resolve(value: T, params: P): U {
        const input = this.prepare ? this.prepare.apply(value, params) : value;
        const output = this.convert(input, params);
        return this.finalize ? this.finalize.apply(output, params) : output;
    }

    abstract convert(value: T, params: P): U;
}
