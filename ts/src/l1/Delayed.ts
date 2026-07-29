// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Transform } from './Transform';
import { Parameters } from './Parameters';

export class Delayed<T, P extends Parameters = Parameters>
    implements Transform<T, P> {

    private instance: Transform<T, P> | undefined;

    constructor(private readonly factory: new () => Transform<T, P>) {}

    acquire(value: T, params: P): T {
        if (!this.instance) {
            this.instance = new this.factory();
        }
        return this.instance.apply(value, params);
    }

    apply(value: T, params: P): T {
        return this.acquire(value, params);
    }
}
