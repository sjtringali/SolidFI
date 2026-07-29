// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Converter } from './Converter';
import { Parameters } from './Parameters';

export class Deferred<T, U, P extends Parameters = Parameters>
    implements Converter<T, U, P> {

    private instance: Converter<T, U, P> | undefined;

    constructor(private readonly factory: new () => Converter<T, U, P>) {}

    acquire(value: T, params: P): U {
        if (!this.instance) {
            this.instance = new this.factory();
        }
        return this.instance.resolve(value, params);
    }

    resolve(value: T, params: P): U {
        return this.acquire(value, params);
    }
}
